#include "debug.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <vector>
#include "util.h"
#include "ioutil.h"
#include "util.h"

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
		int dst_id = (*it)[coord] % world_size;
		division_vector[dst_id].push_tuple(*it);
	}

	return division_vector;
}


std::vector<Relation<int>> distributed_join(Relation<int> &rel1,
		      Relation<int> &rel2,
		      std::vector<int> &vars1,
		      std::vector<int> &vars2)
{
	mpi::communicator world;

	std::vector<Relation<int>> div1; 
	std::vector<Relation<int>> div2;
	if (world.rank() == 0) {
		auto common_vars = common_elems(vars1, vars2);
		int coord1 = 0,
		coord2 = 0;
		for (std::size_t i = 0; i < vars1.size(); i++)
			if (vars1[i] == common_vars[0])
				coord1 = i;
		for (std::size_t i = 0; i < vars2.size(); i++)
			if (vars2[i] == common_vars[0])
				coord2 = i;

		div1 = divide_tuples(rel1, coord1);
		div2 = divide_tuples(rel2, coord2);
	}

	Relation<int> subrel1;
	Relation<int> subrel2;
	
	mpi::scatter(world, div1, subrel1, 0);
	mpi::scatter(world, div2, subrel2, 0);

	auto partial_result = join(subrel1,
				   subrel2,
				   vars1,
				   vars2);

	std::vector<Relation<int>> result;
	gather(world, partial_result, result, 0);
	
	return result;
}

int main() {
	using namespace std;
	mpi::environment env;
	mpi::communicator world;
	Relation<int> r1(2);
	Relation<int> r2(2);

	vector<int> v1{0, 1};
	vector<int> v2{1, 2};

	string f1 = "input1.txt";
	string f2 = "input2.txt";

	if (world.rank() == 0) {
		read_file(f1, r1);
		read_file(f2, r2);
	}

	auto result = distributed_join(r1, r2, v1, v2);

	if (world.rank() == 0) {
		for (auto rel : result)
			for (auto it = rel.begin(); it != rel.end(); it++)
				pv(*it);
	}

	return 0;
}
