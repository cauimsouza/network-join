/*
*
* this file tests triangle detection for different join methods
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

const string INPUTS_PATH("tests/triangles/inputs/");
const string ANSWERS_PATH("tests/triangles/outputs/");
	
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
	for(int i=0; i<3; i++)
		rel_namesv.push_back(filename);
	varsv.push_back(std::vector<int>{0,1});
	varsv.push_back(std::vector<int>{1,2});
	varsv.push_back(std::vector<int>{2,0});	
	
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
		cout<<"We detected "<<result.size()<<" triangles"<<endl;
	}


	return 0;
}
