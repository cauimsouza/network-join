/*
*
* this file tests the join operation for the different join methods
*/
#include <fstream>
#include <vector>
#include <map>
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

void throw_error(string s =
	"Usage: mpirun -np <number of processes> bin/test_join <name of input> <sequential, normal_distrib, optimized_distrib, hypercube> [<mod_hash, mult_hash, murmur_hash>]"){
	mpi::communicator world;
	if(world.rank() == constants::ROOT){
			cout<<"###################"<<endl;
			cout<<s<<endl;
			cout<<"###################"<<endl;
	}
	exit(-1);
}

int main(int argc, char* argv[]) {

    mpi::environment env(argc, argv);   
    mpi::communicator world;
    std::vector<string> list_names_algorithms = {"sequential", "normal_distrib", "optimized_distrib", "hypercube"};
    std::map<string, HashMethod> list_names_hash;
    list_names_hash[string("mod_hash")]= HashMethod::ModHash; 
    list_names_hash[string("mult_hash")]= HashMethod::MultHash; 
    list_names_hash[string("murmur_hash")]= HashMethod::MurmurHash;     

	if(argc <3)
		throw_error();
		
	string name_algorithm = string(argv[2]);
	if(find(list_names_algorithms.begin(), list_names_algorithms.end(), name_algorithm)==list_names_algorithms.end())
		throw_error("Invalid algorithm option");

	HashMethod hash_method;
	if(name_algorithm!="sequential"){
		if(argc<4)
			hash_method =  HashMethod::ModHash; // by default
		else
		{
			string name_hash = string(argv[3]);
			if(!list_names_hash.count(name_hash))
				throw_error("Invalid hash method");		
		}
	}	
	
	string filename = INPUTS_PATH+string(argv[1]);
	ifstream input_file(filename);
	if(!input_file)
		throw_error("File not found");

	vector<string> rel_namesv;
	vector< vector<int> > varsv;	
	read_test(filename, rel_namesv, varsv);
	Relation<int> result;	
	vector<int> result_vars;	
	if(name_algorithm == "sequential")
		result = multiway_join(rel_namesv, varsv, result_vars);
	else if(name_algorithm == "normal_distrib")
		result = distributed_multiway_join(rel_namesv, varsv, result_vars, false, hash_method);
	else if(name_algorithm =="optimized_distrib")
		result = distributed_multiway_join(rel_namesv, varsv, result_vars, true, hash_method);
	else if(name_algorithm == "hypercube")
		result = hypercube_distributed_multiway_join(rel_namesv, varsv, result_vars, hash_method);			
	

	if (world.rank() == constants::ROOT) {
		pv(result_vars);
		cout << endl;
		for (auto it = result.begin(); it != result.end(); it++)
			pv(*it);

		
		string answer_filename = ANSWERS_PATH+string(argv[1]);
		ifstream answer_file(answer_filename);
		if(!answer_file) // end program if there is no verification file
			return 0;

		// if there is exists a verification file, check if the answer is right by counting tuples
		answer_file.close();
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
