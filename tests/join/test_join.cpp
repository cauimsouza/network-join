/*
*
* this file tests the join operation for the different join methods
*/
#include <fstream>
#include <vector>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "ioutil.h"
#include "mpiutil.h"
#include "util.h"
#include "debug.h"

using namespace std;

const string RELATIONS_PATH("tests/join/relations/");
const string INPUTS_PATH("tests/join/inputs/");
const string ANSWERS_PATH("tests/join/outputs/");

void read_test(const string filename, vector<string>& rel_namesv, vector<vector<int>>& varsv)
{
	ifstream f(filename);
	if(!f)
	{
		mpi::communicator world;
		if(world.rank()==constants::ROOT){
			cout<<"###################"<<endl;
			cout<<"File not found, terminating program"<<endl;
			cout<<"###################"<<endl;	
		}
		exit(-1);
	}
	int num_rels;
	f>>num_rels;
	rel_namesv.resize(num_rels); varsv.resize(num_rels);
	for(int i=0; i<num_rels; i++)
	{
		int arity; f>>arity; varsv[i].resize(arity);
		string rel_filename; f>>rel_filename;
		rel_namesv[i]=RELATIONS_PATH+rel_filename;
		for(int j=0; j<arity; j++)
			f>>varsv[i][j];
	}
	f.close();
}

int main(int argc, char* argv[]) {

	mpi::communicator world;
	mpi::environment env;

	if(argc !=3)
	{
		if(world.rank() == constants::ROOT){
			cout<<"###################"<<endl;
			cout<<"Usage: mpirun -np <number of processes> bin/test_join <name of input> <0:sequential, 1:normal distrib, 2:optimized distrib, 3: hypercube>"<<endl;
			cout<<"###################"<<endl;
		}
		return -1;
	}
	
	vector<string> rel_namesv;
	vector< vector<int> > varsv;

	
	read_test(INPUTS_PATH+string(argv[1]), rel_namesv, varsv);


	vector<int> result_vars;
	int algorithm_option = atoi(argv[2]);
	Relation<int> result;
	switch(algorithm_option){
		case 0:
			result = multiway_join(rel_namesv, varsv, result_vars);
			break;
		case 1:
			result = distributed_multiway_join(rel_namesv, varsv, result_vars, false);
			break;
		case 2:
			result = distributed_multiway_join(rel_namesv, varsv, result_vars, true);
			break;
		case 3:
			result = hypercube_distributed_multiway_join(rel_namesv, varsv, result_vars);
			break;
		default:
			if(world.rank() == constants::ROOT){
			cout<<"###################"<<endl;
			cout<<"Wrong algorithm option. <0:sequential, 1:normal distrib, 2:optimized distrib, 3: hypercube>"<<endl;
			cout<<"###################"<<endl;
			}
			return -1;
	}


	if (world.rank() == constants::ROOT) {
		pv(result_vars);
		cout << endl;
		for (auto it = result.begin(); it != result.end(); it++)
			pv(*it);

		// verify answer

		string answer_filename = ANSWERS_PATH+string(argv[1]);
		Relation<int> right_answer(read_arity(answer_filename));
		read_relation(answer_filename, right_answer);

		map<vector<int>, int> m1, m2;
		for(auto tuple : right_answer)
		{
			if(m1.count(tuple))
				m1[tuple]++;
			else
				m1[tuple]=1;
		}

		for(auto tuple : result)
		{
			if(m2.count(tuple))
				m2[tuple]++;
			else
				m2[tuple]=1;
		}
		cout<<"------"<<endl;
		cout<<right_answer<<endl;
		cout<<"------"<<endl;
		if (m1.size() == m2.size() && std::equal(m1.begin(), m1.end(),m2.begin())){
			cout<<"Answer is CORRECT"<<endl;
		}
		else
			cout<<"Answer is WRONG"<<endl;
	}


	return 0;
}
