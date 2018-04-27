#include <algorithm>
#include <vector>
#include <utility>
#include "util.h"
#include "ioutil.h"

std::vector<int> get_perm(const std::vector<int>& var_vect,
			  const std::vector<int>& common_vect)
{
	std::vector<int> map_vect(var_vect.size(), 0);
	for (std::size_t i = 0; i < var_vect.size(); i++) {
		int id = std::lower_bound(common_vect.begin(),
				common_vect.end(), var_vect[i])
			 - common_vect.begin();
		map_vect[i] = id;
	}

	std::vector<int> freq_vect(common_vect.size() + 1, 0);
	for (auto i : map_vect)	freq_vect[i]++;
	for (std::size_t i = 1; i < freq_vect.size(); i++) 
		freq_vect[i] +=	freq_vect[i-1];
	for (std::size_t i = freq_vect.size() - 1; i > 0; i--)
		freq_vect[i] = freq_vect[i-1];
	freq_vect[0] = 0;

	std::vector<int> perm(var_vect.size(), 0);
	for (std::size_t i = 0; i < perm.size(); i++) 
		perm[i] = freq_vect[map_vect[i]]++;

	return perm;
}

/*
 * Returns a vector that maps each index i in common_vars
 * to an index j in vars such that these indexes represent the 
 * same symbolic variable
 *
 * @param vars vector containing a set of variables
 * @param common_vars vector containing a subset of variables of vars
 * @return a mapping from each index in common_vars to a index in
 * 	in vars representing the same variable
 */
std::vector<int> index_common_variables(const std::vector<int>& vars,
					const std::vector<int>& common_vars)
{
	std::vector<int> id_vector(common_vars.size(), 0);
	for (std::size_t i = 0; i < vars.size(); i++) {
		int id = std::lower_bound(common_vars.begin(),
				common_vars.end(), vars[i]) -
				common_vars.begin();
		if (id < common_vars.size()) id_vector[id] = i;
	}

	return id_vector;
}

int compare_assignments(const std::vector<int>& tpl1, const std::vector<int>& matching1,
		const std::vector<int>& tpl2, const std::vector<int>& matching2)
{
	for (std::size_t i = 0; i < matching1.size(); i++) {
		if (tpl1[matching1[i]] < tpl2[matching2[i]])	return -1;
		else if (tpl1[matching1[i]] > tpl2[matching2[i]]) return 1;
	}

	return 0;
}

bool consistent(const std::vector<int>& tpl, const std::vector<int>& vars)
{
	for (std::size_t i = 1; i < vars.size(); i++) {
		for (int j = i - 1; j >= 0; j--)
			if (vars[i] == vars[j]) {
				if (tpl[i] == tpl[j])	break;
				else	return false;
			}
	}
	return true;
}

std::vector<std::pair<Relation<int>::tuple_t, Relation<int>::tuple_t>> join(Relation<int>& rel1, Relation<int>& rel2, const std::vector<int>& var1,
const std::vector<int>& var2)
{
	auto common_vars = common_elems(var1, var2);	
	auto matching1 = index_common_variables(var1, common_vars);
	auto matching2 = index_common_variables(var2, common_vars);

	sort(rel1, get_perm(var1, common_vars));
	sort(rel2, get_perm(var2, common_vars));

	std::vector<std::pair<Relation<int>::tuple_t, Relation<int>::tuple_t>> join_vector;
	auto it1 = rel1.begin();
	auto it2 = rel2.begin();
	while (it1 != rel1.end() && it2 != rel2.end()) {
		int comp = compare_assignments(*it1, matching1, *it2, matching2);

		if (comp < 0) it1++;
		else if (comp > 0) it2++;
		else if (!consistent(*it1, var1)) it1++;
		else if (!consistent(*it2, var2)) it2++;
		else {
			for (auto it3 = it2; it3 != rel2.end() && 
			     compare_assignments(*it1, matching1, *it3, matching2) == 0;
			     it3++)
			     join_vector.push_back(std::make_pair(*it1, *it3));	

			it1++;
		}
	}

	return join_vector;
}

// TEST JOIN FUNCTION
// using namespace std;
//
// int main() {
// 	string f1 = "input1.txt";
// 	string f2 = "input2.txt";
// 	Relation<int> r1(3);
// 	Relation<int> r2(2);
// 	read_file(f1, r1);
// 	read_file(f2, r2);
//
// 	string f3 = "output1.txt";
// 	string f4 = "output2.txt";
//
// 	write_file(f3, r1);
// 	write_file(f4, r2);
//
// 	vector<int> vars1{2, 1, 2};
// 	vector<int> vars2{2, 3};
//
// 	auto ans = join(r1, r2, vars1, vars2);
//
// 	for (auto par : ans) {
// 		auto v1 = par.first;
// 		for (auto i : v1) cout << i << " ";
// 		cout << "| ";
// 		auto v2 = par.second;
// 		for (auto i : v2) cout << i << " ";
// 		cout << endl;
// 	}
//
// 	return 0;
// }
