#include <algorithm>
#include <vector>
#include <utility>
#include <set>
#include "util.h"
#include "ioutil.h"
#include "debug.h"

/*
 * Takes a vector of integers var_vect and a subvector
 * common_vect, and generates a list of priorities for
 * each number in var_vect such that:
 * - every number has a different priority
 * - if x is in common_vect and y is not, then x will
 *   receive higher priority (lower number)
 * - if x and y appear in common_vect, the one who appears
 *   first will receive higher priority
 * 
 * @param var_vect vector of integers
 * @param common_vect vector of integers, subvector of var_vect
 * 	  WARNING: common_vect must be ordered!!!
 * @return a permutation of [0, var_vect.size()) as described above 
 */
std::vector<int> get_perm(const std::vector<int>& var_vect,
			  const std::vector<int>& common_vect)
{
	std::vector<int> map_vect(var_vect.size(), 0);
	for (std::size_t i = 0; i < var_vect.size(); i++) {
		auto it = std::lower_bound(common_vect.begin(),
				common_vect.end(), var_vect[i]);
		int id = it - common_vect.begin();

		if (it == common_vect.end() || *it != var_vect[i])
			id = common_vect.size();

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
		perm[freq_vect[map_vect[i]]++] = i;

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

/*
 * This function compares assignments of two tuples based on the order
 * given by the matching vectors.
 *
 * @param tpl1 assignment of first tuple
 * @param matching1 order in which elements in tpl1 should be considered
 * 	in the comparison
 * @param tpl2 assingment of second tuple
 * @param matching2 order in which elements in tpl2 should be considered
 * 	in the comparison
 * @return
 * - 0 if the tuples are equal according to the orders determined by the
 *   matching vectors
 * - 1 if the first tuple is considered to be greater than the second
 * - -1 if the first tuple is considered to be smaller than the second
 *
 * Remarks:
 * - the matching vectors do not need to be exhaustive. It might be possible 
 *   not to consider some elements (or any of them).
 * - the matching vectors must have the same size.
 */
int compare_assignments(const std::vector<int>& tpl1, const std::vector<int>& matching1,
		const std::vector<int>& tpl2, const std::vector<int>& matching2)
{
	for (std::size_t i = 0; i < matching1.size(); i++) {
		if (tpl1[matching1[i]] < tpl2[matching2[i]])	return -1;
		else if (tpl1[matching1[i]] > tpl2[matching2[i]]) return 1;
	}

	return 0;
}

/*
 * Checks if an assignment to a tuple is consistent with the assignment of
 * variables. Example:
 * assignment (1, 2, 3) is consistent with the tuple (x_1, x_2, x_3) but
 * not with the tuple (x_1, x_2, x_1) because the first and last elements
 * of the assingment have different values while they are values of the same
 * variable.
 *
 * @param tpl assignment of values to variables in vars
 * @param vars variables
 * @return true if the assignment tpl is consistent with variables in vars
 */
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

/*
 * Returns a (sorted) vector containing each element present in either
 * vars1 or vars2, exactly once
 *
 * @param vars1 first vector of integers
 * @param vars2 second vector of integers
 * @return a vector containing each element in the union of 
 * 	vars1 and vars2, exactly once
 */
std::vector<int> get_unique_vars(const std::vector<int>& vars1,
				 const std::vector<int>& vars2)
{
	std::set<int> vars_set{vars1.begin(), vars1.end()};
	vars_set.insert(vars2.begin(), vars2.end());
	std::vector<int> unique_vars{vars_set.begin(), vars_set.end()};
	return unique_vars;
}

/*
 * Merge two tuples of assignments based on the variables associated
 * with each one.
 *
 * @param tpl1 first tuple of assignment
 * @param tpl2 second tuple of assignment
 * @param vars1 variables associated with first tuple
 * @param vars2 variables associated with second tuple
 * @param unique_vars vector sorted in increasing order containing
 * 	the variables appearing in either vars1 or vars2, exactly
 * 	once
 * @return a tuple containing assignments to the variables in
 * 	unique_vars in the same order
 */
std::vector<int> merge_reduce_tpls(const std::vector<int>& tpl1,
				   const std::vector<int>& tpl2,
				   const std::vector<int>& vars1,
				   const std::vector<int>& vars2,
				   const std::vector<int>& unique_vars)
{
	std::vector<int> tpl(unique_vars.size());	
	std::vector<bool> vars_filled(unique_vars.size(), false);
	
	std::size_t counter = 0;
	for (std::size_t i = 0; i < vars1.size() && counter < tpl.size(); i++) {
		int id = std::lower_bound(unique_vars.begin(),
					  unique_vars.end(),
					  vars1[i]) -
			 unique_vars.begin();
		if (!vars_filled[id]) {
			vars_filled[id] = true;
			tpl[id] = tpl1[i];
			counter++;
		}
	}

	for (std::size_t i = 0; i < vars2.size() && counter < tpl.size(); i++) {
		int id = std::lower_bound(unique_vars.begin(),
					  unique_vars.end(),
					  vars2[i]) -
			 unique_vars.begin();
		if (!vars_filled[id]) {
			vars_filled[id] = true;
			tpl[id] = tpl2[i];
			counter++;
		}
	}

	return tpl;
}

/*
 * Performs join operation in two relations.
 *
 * @param rel1 first relation
 * @param rel2 second relation
 * @param vars1 tuple of variables for first relation
 * @param vars2 tuple of variables for second relation
 * @return result of join operation
 */
Relation<int> join(Relation<int>& rel1,
		   Relation<int>& rel2,
		   const std::vector<int>& vars1,
     		   const std::vector<int>& vars2)
{
	auto common_vars = common_elems(vars1, vars2);	
	auto unique_vars = get_unique_vars(vars1, vars2);
	auto matching1 = index_common_variables(vars1, common_vars);
	auto matching2 = index_common_variables(vars2, common_vars);

	sort(rel1, get_perm(vars1, common_vars));
	sort(rel2, get_perm(vars2, common_vars));

	Relation<int> join_rel(unique_vars.size());

	auto it1 = rel1.begin();
	auto it2 = rel2.begin();
	while (it1 != rel1.end() && it2 != rel2.end()) {
		int comp = compare_assignments(*it1, matching1, *it2, matching2);

		if (!consistent(*it1, vars1)) it1++;
		else if (!consistent(*it2, vars2)) it2++;
		else if (comp < 0) it1++;
		else if (comp > 0) it2++;
		else {
			for (auto it3 = it2; it3 != rel2.end() && 
			     compare_assignments(*it1, matching1, *it3, matching2) == 0;
			     it3++)
			     join_rel.push_tuple(merge_reduce_tpls(*it1, *it3,
			     			     vars1, vars2, unique_vars));	

			it1++;
		}
	}

	return join_rel;
}

/*
 * Performs join operation for multiple relations.
 *
 * @param rel_namesv vector containing the names of relation files
 * @param varsv	vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @return result of join operation as a new relation
 */
Relation<int> multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars)
{
	
	Relation<int> result_rel(read_arity(rel_namesv.front()));
	read_relation(rel_namesv.front(),  result_rel);
	result_vars = varsv.front();

	auto rel_it = ++rel_namesv.begin();
	auto vars_it = ++varsv.begin();
	for(;rel_it != rel_namesv.end();rel_it++,vars_it++) {
		Relation<int> buff_rel(read_arity(*rel_it));
		read_relation(*rel_it, buff_rel);
		result_rel = join(result_rel, buff_rel, result_vars, *vars_it);
		result_vars = get_unique_vars(result_vars, *vars_it);	
	}

	return result_rel;
}

