/*
*
* this file tests distributed multiway join with no optimization
*/
#include <vector>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "ioutil.h"
#include "mpiutil.h"
#include "util.h"
#include "debug.h"

int main() {
	using namespace std;
	mpi::environment env;
	mpi::communicator world;
	const int root = 0;

	Relation<int> r1(2);
	Relation<int> r2(2);
	Relation<int> r3(2);

	vector<int> v1{0, 1};
	vector<int> v2{1, 2};
	vector<int> v3{2, 3};

	string f1 = "tests/mpi_simple/test_cases/input1.txt";
	string f2 = "tests/mpi_simple/test_cases/input2.txt";
	string f3 = "tests/mpi_simple/test_cases/input3.txt";

	vector<string> relv; relv.push_back(f1);relv.push_back(f2);relv.push_back(f3);
	vector< vector<int> > varsv{v1, v2, v3};
	vector<int> result_vars;
	auto result = distributed_multiway_join(relv, varsv, result_vars, false);
	if (world.rank() == root) {
		pv(result_vars);
		cout << endl;
		for (auto it = result.begin(); it != result.end(); it++)
			pv(*it);
	}

	// auto result = distributed_join(r1, r2, v1, v2);
        //
	// for (auto it = result.begin(); it != result.end(); it++)
	// 	pv(*it);

	return 0;
}
