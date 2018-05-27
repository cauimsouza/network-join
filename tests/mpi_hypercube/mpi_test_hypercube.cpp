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

int main(int argc, char* argv[]) {
	using namespace std;

	mpi::communicator world;
	mpi::environment env;

	const string PATH_INPUTS("tests/mpi_hypercube/test_cases/");
	
	if(argc !=2)
	{
		cout<<"Enter the name of the desired input";
		return -1;
	}
	
	
	vector<int> v1{0, 1}, v2{2, 3}, v5{4, 0}, v3{1, 2}, v4{3, 4};
	string f1, f2, f3, f4, f5;
	f1=f2=f3=f4=f5= PATH_INPUTS+string(argv[1]);

	vector<string> rel_namesv{f1,f2};
	vector< vector<int> > varsv{v1,v3};
	vector<int> result_vars;
	auto result =hypercube_distributed_multiway_join(rel_namesv, varsv, result_vars);
	

	if (world.rank() == constants::ROOT) {
		pv(result_vars);
		cout << endl;
		for (auto it = result.begin(); it != result.end(); it++)
			pv(*it);
	}


	return 0;
}
