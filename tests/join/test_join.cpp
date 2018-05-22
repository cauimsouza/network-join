/*
* this file tests the join function
*/

#include <iostream>
#include <algorithm>
#include "relation.h"
#include "ioutil.h"
#include "util.h"
#include <string>

int main(int argc, char** argv)
{
	if(argc!=2)
	{
		std::cout<<"Usage: bin/test_join <test name>";
		return -1;
	}
	std::string test_path="tests/join/test_cases/";
	std::string test_name(argv[1]); // Ex: "both_on_top"
	std::string input1 = test_path+test_name+"_in1.txt",
			input2 = test_path+test_name+"_in2.txt",
			output = test_path+test_name+"_out.txt";

	auto sz1= read_arity(input1), sz2 = read_arity(input2);
	Relation<int> r1(sz1); Relation<int> r2(sz2);
	read_file(input1, r1); read_file(input2, r2);

	std::vector<int> vars1={1, 2};
	std::vector<int> vars2={1, 3};

	Relation<int> r = join(r1, r2, vars1,  vars2);
	std::cout<<r;
	std::cout<<"-----------"<<std::endl;
	system((std::string("cat ")+output).data());

	return 0;	
}
