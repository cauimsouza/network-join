#include <iostream>
#include <algorithm>
#include "relation.h"
#include "ioutil.h"
#include "util.h"
#include <string>

using namespace std;

int main()
{
	string filename = "input.txt";
	auto sz = read_arity(filename);
	Relation<int> rel(sz);
	
	read_file(filename, rel);
	//cin >> rel;
	cout << rel.size() << " elements" << endl;
	cout << rel << endl;
	
	vector<int> permut(2);
	permut[0] = 0;
	permut[1] = 1;
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

	cout << "hello" << endl;
	vector<int> A{2, 1, 3, 4, 1};
	vector<int> B{5, 6, 1, 7, 2};
	auto C = common_elems(A, B);
	for (auto i : C)
		cout << i << " ";
	cout << endl;

	auto D = get_perm(B, C); 
	for (auto i : D)
		cout << i << " ";
	cout << endl;

	return 0;	
}
