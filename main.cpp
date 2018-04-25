#include <iostream>
#include <algorithm>
#include "relation.h"
#include "ioutil.h"
#include "util.h"
#include <string>

using namespace std;

int main()
{
	Relation<int, 2> rel;
	string filename = "input.txt";
	
	read_file(filename, rel);
	//cin >> rel;
	cout << rel.size() << " elements" << endl;
	
	Relation<int, 2>::tuple_t permut;
	permut[0] = 1;
	permut[1] = 0;
	sort(rel, permut);
	/*
	for (auto it = rel.begin(); it != rel.end(); it++) {
		cout << (*it)[0] << (*it)[1] << endl;
	}
	*/
	cout << rel;

//	sort(rel.begin(), rel.end());

	string ofilename = "output.txt";
	write_file(ofilename, rel);

	return 0;	
}
