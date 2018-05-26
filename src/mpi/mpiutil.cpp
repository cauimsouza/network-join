#include <vector>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "ioutil.h"
#include "util.h"
#include "hash.h"
#include "debug.h"

namespace mpi = boost::mpi;

/*
 * This function takes a relation of integer tuples and divide it
 * into nproc relations, where nproc is the number of processes.
 * A tuple tpl is assigned to the relation of index equals to 
 * tpl[coord] % nproc, where tpl[coord] is the value of the 
 * coord-th coordinate of the tpl.
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
	
	for (auto it = rel.begin(); it != rel.end(); it++) {
		int dst_id = mod_hash((*it)[coord], world_size);
		// int dst_id = mult_hash((*it)[coord], world_size);
		// int dst_id = (*it)[coord] % world_size;
		division_vector[dst_id].push_tuple(*it);
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
	const int root = 0;

	std::vector<Relation<int>> div1; 
	std::vector<Relation<int>> div2;
	if (world.rank() == root) {
		auto common_vars = common_elems(vars1, vars2);
		int coord1 = 0,
		coord2 = 0;
		if (common_vars.size() > 0) {
			for (std::size_t i = 0; i < vars1.size(); i++)
				if (vars1[i] == common_vars[0])
					coord1 = i;
			for (std::size_t i = 0; i < vars2.size(); i++)
				if (vars2[i] == common_vars[0])
					coord2 = i;
		}

		div1 = divide_tuples(rel1, coord1);
		div2 = divide_tuples(rel2, coord2);

		rel1.clear();
		rel2.clear();
	}

	Relation<int> subrel1;
	Relation<int> subrel2;
	
	mpi::scatter(world, div1, subrel1, root);
	mpi::scatter(world, div2, subrel2, root);

	std::cout << "process " << world.rank() << \
		", " << subrel1.size() << ", " << \
		subrel2.size() << std::endl;

	auto partial_result = join(subrel1,
				   subrel2,
				   vars1,
				   vars2);

	Relation<int> result;
	reduce(world, partial_result, result, concatenate_functor<int>(), root);

	return result;
}

/*
 * Performs join operation for multiple relations
 * in a distributed fashion using Boost's MPI
 * implementation
 *
 * @param relv vector of relations
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join(std::vector<Relation<int>>& relv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars)
{
	auto rel_it = relv.begin();
	auto vars_it = varsv.begin();
	Relation<int> result_rel = *rel_it;
	result_vars = *vars_it;

	rel_it++;
	vars_it++;
	int round = 0;
	while (rel_it != relv.end()) {
		std::cout << round++ << " round" << std::endl;
		result_rel = distributed_join(result_rel, *rel_it, result_vars, *vars_it);
		result_vars = get_unique_vars(result_vars, *vars_it);

		rel_it++;
		vars_it++;
	}

	return result_rel;
}

