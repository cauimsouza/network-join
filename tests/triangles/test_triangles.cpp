/*
* this file tests triangle detection for sequential algorithm
*/

#include <iostream>
#include <algorithm>
#include "relation.h"
#include "ioutil.h"
#include "util.h"
#include <string>

int main(int argc, char** argv)
{
	std::string test_path="tests/triangles/test_cases/";
	std::string test_name="1";
	std::string input = test_path+std::string("in")+test_name+".txt",
				output = test_path+std::string("out")+test_name+".txt";

	auto sz= read_arity(input);
	Relation<int> graph(sz); 
	read_relation(input, graph); 


	// setting list of variables
	std::vector<std::vector<int> > list_vars;
	list_vars.push_back(std::vector<int>{1, 2});
	list_vars.push_back(std::vector<int>{2, 1});
	list_vars.push_back(std::vector<int>{2, 3});
	list_vars.push_back(std::vector<int>{3, 2});
	list_vars.push_back(std::vector<int>{1, 3});
	list_vars.push_back(std::vector<int>{3, 1});	


	// setting list of relations (they're  all the same and equal to graph)
	std::vector<Relation<int> > list_relations;
	for(int i=0; i<list_vars.size(); list_relations.push_back(graph), i++);


	//vector of resulting variables
	std::vector<int> result_vars;


	//Relation<int> triangles = join(graph, graph, list_vars[0], list_vars[1]);
	Relation<int> triangles = multiway_join(list_relations, list_vars, result_vars);

	for(auto i : result_vars)
		std::cout<<"x"<<i<<" ";
	std::cout<<std::endl;
	std::cout<<triangles;
	std::cout<<"-----------"<<std::endl;
	system((std::string("cat ")+output).data());

	return 0;	
}
