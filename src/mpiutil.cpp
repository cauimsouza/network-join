#include <vector>
#include <functional>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "ioutil.h"
#include "mpiutil.h"
#include "util.h"
#include "hash.h"
#include "debug.h"

/*
 * This function takes a relation of integer tuples and divides it
 * into nproc relations, where nproc is the number of processes.
 * A tuple tpl is assigned to a certain relation according to a 
 * reference variable of the tuple (coord) and a given hash function.
 *
 * @param rel original relation to be divided into nproc relations
 * @param coord coordinate according to which the tuples will be
 * 	  assigned to the different processes
 * @result a vector of truple whose concatenation is equal to the
 * 	   original relation
 */
std::vector<Relation<int>> divide_tuples(Relation<int> &rel, int coord)
{
	mpi::communicator world;
	int world_size = world.size();

	std::vector<Relation<int>> division_vector(world_size, Relation<int>(rel.get_arity()));
	
	if(coord==constants::NONE) // send every tuple to constants::ROOT
		division_vector[constants::ROOT] = rel;
	else{
		for (auto it = rel.begin(); it != rel.end(); it++) {
			int dst_id = mod_hash((*it)[coord], world_size);
			// int dst_id = mult_hash((*it)[coord], world_size);
			// int dst_id = (*it)[coord] % world_size;
			division_vector[dst_id].push_tuple(*it);
		}	
	}
	

	return division_vector;
}

template<typename T>
struct concatenate_functor
{
	Relation<T>& operator()(Relation<T> &r1, Relation<T> &r2) {
		return r1.concatenate(r2);
	}
};

/*
 * Specialization of functor to allow
 * faster reduce algorithm
 */
namespace boost { namespace mpi {
	template<typename T>
	struct is_commutative<concatenate_functor<T>, Relation<T> >
	: mpl::true_ { };
} }

/* Performs join operation in a distributed fashion
 * using Boost's MPI interface.
 *
 * @param rel1 first relation
 * @param rel2 second relation
 * @param vars1 tuple of variables for first relation
 * @param vars2 tuple of variables for second relation
 * @return result of join operation
 */
Relation<int> distributed_join(Relation<int> &rel1,
		      Relation<int> &rel2,
		      std::vector<int> &vars1,
		      std::vector<int> &vars2)
{
	mpi::communicator world;

	std::vector<Relation<int>> div1; 
	std::vector<Relation<int>> div2;
	if (world.rank() == constants::ROOT) {
		auto common_vars = common_elems(vars1, vars2);
		int coord1,coord2;
		if (common_vars.size() > 0) {
			coord1=coord2 = 0;
			for (std::size_t i = 0; i < vars1.size(); i++)
				if (vars1[i] == common_vars[0])
					coord1 = i;
			for (std::size_t i = 0; i < vars2.size(); i++)
				if (vars2[i] == common_vars[0])
					coord2 = i;
		}
		else // no common variables
		{
			coord1=coord2=constants::NONE;
		}

		div1 = divide_tuples(rel1, coord1);
		div2 = divide_tuples(rel2, coord2);

		rel1.clear();
		rel2.clear();
	}

	Relation<int> subrel1;
	Relation<int> subrel2;
	
	mpi::scatter(world, div1, subrel1, constants::ROOT);
	mpi::scatter(world, div2, subrel2, constants::ROOT);

	auto partial_result = join(subrel1,
				   subrel2,
				   vars1,
				   vars2);

	Relation<int> result;
	reduce(world, partial_result, result, concatenate_functor<int>(), constants::ROOT);

	return result;
}


/*
 * Auxiliary method for distributed_multiway_join. Performs multijoin
 * in a non-optimized way (processes send their partial results to the
 * root before the next join).
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join_simple(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars)
{
	mpi::communicator world;
	auto rel_it = rel_namesv.begin();
	auto vars_it = varsv.begin();
	auto arity = read_arity(rel_namesv.front());
	Relation<int> result_rel(arity);
	Relation<int> aux_rel;
	read_relation(*rel_it, result_rel);
	result_vars = *vars_it;

	rel_it++;
	vars_it++;
	int round = 0;
	while (rel_it != rel_namesv.end()) {
		if (world.rank() == constants::ROOT) {
			aux_rel.clear();
			aux_rel.set_arity(read_arity(*rel_it));
			read_relation(*rel_it, aux_rel);
		}
		result_rel = distributed_join(result_rel, aux_rel, result_vars, *vars_it);
		result_vars = get_unique_vars(result_vars, *vars_it);

		rel_it++;
		vars_it++;
	}

	return result_rel;
}

/*
 * Auxiliary method for distributed_multiway_join. Performs multijoin
 * in an optimized way (processes scatter their partial results directly
 * to the appropriate machines for the next join).
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join_forwarding(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars)
{
	mpi::communicator world;
	auto rel_it = rel_namesv.begin();
	auto vars_it = varsv.begin();
	Relation<int> buff_rel; // stores the read relation
	Relation<int> left_subrel(read_arity(rel_namesv.front())); // stores the remainings of the previous join 
	std::vector<int> left_vars=*vars_it; // cumulates the unique variables as we go on		
	if(world.rank()==constants::ROOT){ // the root process starts with the entire first relation, which will be distributed in Part 2 
		read_relation(*rel_it, left_subrel);
	} 	
	rel_it++; // start from second relation
	vars_it++;	
	// for each relation, calculate distributed binary join with optimization
	for(int curr_division_var = constants::NONE, prev_division_var = constants::NONE; rel_it != rel_namesv.end();){ 
		///////// Pt1: scatter the read relation to right_subrel
		std::vector<Relation<int> > divided_buff_rel; std::vector<int> & right_vars = *vars_it;
		if (world.rank() == constants::ROOT) { 
			buff_rel.clear();
			buff_rel.set_arity(read_arity(*rel_it));
			read_relation(*rel_it, buff_rel);		
			auto common_vars = common_elems(left_vars, right_vars); //calculate common variables
			// decide reference variable for division
			if(find(common_vars.begin(), common_vars.end(), prev_division_var)!=common_vars.end()) // if our division is still valid, keep it
				curr_division_var = prev_division_var;
			else // otherwise we need to find new reference variable
			{
				if(common_vars.size()>0)
					curr_division_var = common_vars[0];
				
				else // there may be none
					curr_division_var = constants::NONE;				
			}
			int division_index = (curr_division_var==constants::NONE)? 
				constants::NONE : std::find(right_vars.begin(),right_vars.end(),curr_division_var)-right_vars.begin();
			divided_buff_rel = divide_tuples(buff_rel, division_index);
		}		
		mpi::broadcast(world, curr_division_var, constants::ROOT);
		Relation<int> right_subrel;
		mpi::scatter(world, divided_buff_rel, right_subrel, constants::ROOT);

		///////// Pt2: scatter the remainings (left_subrel) to the appropriate machines
		if(curr_division_var != prev_division_var) // we only have to scatter if previous division is now invalid
		{
			Relation<int> prev_left_subrel = left_subrel; //backup data, so we can distribute it
			std::vector<Relation<int>> divided_prev_left_rel;
			int division_index = (curr_division_var==constants::NONE)?
				constants::NONE : std::find(left_vars.begin(),left_vars.end(),curr_division_var)-left_vars.begin();
			divided_prev_left_rel=divide_tuples(prev_left_subrel, division_index);			
			for(int i=0; i<world.size(); i++){
				mpi::reduce(world, divided_prev_left_rel[i], left_subrel, concatenate_functor<int>(), i);
			}			
		}		
		////////  Pt 3 Calculate the binary join of left_subrel and right_subrel
		left_subrel = join(left_subrel,
					   right_subrel,
					   left_vars,
					   right_vars);	
		// loop update
		rel_it++;
		vars_it++;
		left_vars = get_unique_vars(left_vars, right_vars);
		prev_division_var=curr_division_var;
	}

	Relation<int> result_rel;
	reduce(world, left_subrel, result_rel, concatenate_functor<int>(), constants::ROOT);
	result_vars = std::vector<int>(left_vars);

	return result_rel;
}

/*
 * Performs join operation for multiple relations
 * in a distributed fashion using Boost's MPI
 * implementation
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @param forward flag to enable auto-forward optimization
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars, bool forward)
{
	if(forward)
		return  distributed_multiway_join_forwarding(rel_namesv, varsv, result_vars);
	return distributed_multiway_join_simple(rel_namesv, varsv, result_vars);
}

/*
 * Divides the total number of processes into k factors
 * (where k is the number of distinct variables in the join), 
 * trying to keep such factors close. In particular, we minimize 
 * the sum of the factors using dynamic programming.
 *
 * @param num_procs number of processes
 * @param num_vars number of distinct variales in the multiway join
 * @return vector of factors
 */
std::vector<int> equally_factorize(int num_procs, int num_vars){
	using namespace std; 
	using Cell = pair<int, vector<int> >;

	Cell empty_cell(-1, vector<int>());
	vector<vector<Cell> > memo(num_procs+1, vector<Cell>(num_vars+1, empty_cell));

	
	function<Cell(int,int)> dp = [&dp, &memo](int num_procs, int num_vars){		
		const int INF = numeric_limits<int>().max();
		if (num_vars==1)
        	return Cell(num_procs,vector<int>(1, num_procs));	

        if (memo[num_procs][num_vars].first==-1)
	    {
		    int best_cost = INF;
		    vector<int> best_factors;
		    for(int i=1; i * i <= num_procs; i++){
		        if (num_procs % i == 0){
		            Cell sub_result= dp(num_procs / i, num_vars - 1);
		            if (sub_result.first + i <= best_cost){
		                best_cost = sub_result.first + i;
		                best_factors = sub_result.second; best_factors.push_back(i);
		            }
		        }
			}
		    memo[num_procs][num_vars].first = best_cost;
		    memo[num_procs][num_vars].second = best_factors;
	    } 
    	return memo[num_procs][num_vars];
	};

	return dp(num_procs, num_vars).second;   
}      
/*
 * Function that calculates the indices (0 to num_procs-1) of all processes to whom we should send
 * a certain tuple during the hypercube algorithm
 *
 * @param tuple the tuple we want to send
 * @param vars vector indicating the corresponding vars of the tuple
 * @param address_limits vector with the limits of each coordinate in the vector form of a process' address
 * @param destinations reference to vector where we will store the result
 */
void calculate_destinations(Relation<int>::tuple_t& tuple, std::vector<int>& vars, std::vector<int>& address_limits, 
	 std::vector<int>& destinations){	
	// given a vector (x1, ..., xk) where 0<=xi<mi, we can map it uniquely to  {0, ... , m1*...*mk-1}
	// by doing h(x1, ... , xk) = x1+m1*x2+m1*m2*x3+..., which can be calculated recursively
	// through s_k = s_(k-1)*m_k+x_k

	std::function<void(int, int)> recursive_calc = 
	[&tuple,&vars,&address_limits,&destinations, &recursive_calc](int var_index, int curr_sum){
		if(var_index==-1) // if I've already chosen every entry in the address, curr_sum stores the process rank
			destinations.push_back(curr_sum);
		else if(find(vars.begin(), vars.end(), var_index)!=vars.end()){ // if this is one the variables, use hash to decide x_k
			int coord = find(vars.begin(), vars.end(), var_index)-vars.begin();
			int r = mod_hash(tuple[coord], address_limits[var_index]);
			recursive_calc(var_index-1, r+address_limits[var_index]*curr_sum);
		}
		else
		{
			for(int i=0; i<address_limits[var_index]; i++)
				recursive_calc(var_index-1, i+address_limits[var_index]*curr_sum);
		}

	};
	int num_vars = address_limits.size(); // number of distinct variables
	recursive_calc(num_vars-1, 0);
}
/*
 * Function that splits a relation according to the hypercube algorithm
 *
 * @param rel the relation we want to split
 * @param divided_rel reference to vector of relations where we will store the result,i.e., the splitted relations
 * @param vars vector indicating the corresponding vars of the tuple
 * @param address_limits vector with the limits of each coordinate in the vector form of a process' address
 */
void hypercube_divide_tuples(Relation<int>& rel, std::vector<Relation<int>>& divided_rel,
	std::vector<int>& vars, std::vector<int>& address_limits)
		
{	
		for(auto tuple : rel)
		{
			std::vector<int> destinations;
			calculate_destinations(tuple,vars,address_limits,destinations);
			for(int dest:destinations){
				divided_rel[dest].push_tuple(tuple);
			}
		}
}

/*
 * Performs join operation for multiple relations
 * in a distributed fashion using Boost's MPI
 * implementation, by applying the hypercube algorithm
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @param forward flag to enable auto-forward optimization
 * @return result of join operation as a new relation
 */
Relation<int> hypercube_distributed_multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars)
{
	mpi::communicator world;
	int num_procs = world.size();
	std::vector<int> address_limits;
	if(world.rank()==constants::ROOT){
		result_vars = varsv.front();
		for(auto vars_it=++varsv.begin();vars_it!=varsv.end(); vars_it++)
			result_vars= get_unique_vars(result_vars, *vars_it);
		int num_vars =result_vars.size();
		address_limits = equally_factorize(num_procs, num_vars);
	}
	//divide first relation to initialize local_result_rel
	Relation<int> local_result_rel; std::vector<int> local_vars=varsv.front();
	std::vector<Relation<int>> divided_buff_rel;
	if (world.rank() == constants::ROOT) {
			Relation<int> buff_rel;			
			buff_rel.set_arity(read_arity(rel_namesv.front()));
			read_relation(rel_namesv.front(), buff_rel); 
			divided_buff_rel.assign(num_procs, Relation<int>(buff_rel.get_arity()));
			hypercube_divide_tuples(buff_rel, divided_buff_rel, varsv.front(), address_limits);
	}
	// scatter from divided_buff_rel to local_result_rel
	mpi::scatter(world, divided_buff_rel, local_result_rel, constants::ROOT);

	auto rel_it = ++rel_namesv.begin(); auto vars_it = ++varsv.begin();
	for(;rel_it!=rel_namesv.end();rel_it++,vars_it++) {		
		Relation<int> local_buff_rel;
		if (world.rank() == constants::ROOT) {
			Relation<int> buff_rel;			
			buff_rel.set_arity(read_arity(*rel_it));
			read_relation(*rel_it, buff_rel);
			divided_buff_rel.assign(num_procs, Relation<int>(buff_rel.get_arity()));
			hypercube_divide_tuples(buff_rel, divided_buff_rel, *vars_it, address_limits);
		}	
		//scatter from divided_buff_rel to local_buff_rel
		mpi::scatter(world, divided_buff_rel, local_buff_rel, constants::ROOT);
		//join local_result_rel to the relation that was read and divided
		local_result_rel = join(local_result_rel, local_buff_rel, local_vars, *vars_it);	
		local_vars = get_unique_vars(local_vars, *vars_it);
	}
	// combine all local results
	Relation<int> result_rel;	
	mpi::reduce(world, local_result_rel, result_rel, concatenate_functor<int>(), constants::ROOT);

	return result_rel;
	
}
