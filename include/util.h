/*
 * Created in Palaiseau (France) in 30/05/2018 by:
 * Cauim de S. Lima (cauimsouza@gmail.com)
 * Victor Hugo V. Silva (vitorvianna10@gmail.com)
 */

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include "relation.h"

/* 
 * Sorts a relation according to the order prescribed by perm
 *
 * @param rel relation to sort
 * @param perm permutation of the set {0, ..., rel.arity() -1}
 * 	according to which the tuples in the relation will be
 *	sorted
 */
template<typename T>
void sort(Relation<T>& rel, const std::vector<int> perm)
{
	std::sort(rel.begin(), rel.end(),
	[perm](const typename Relation<T>::tuple_t& A,
	       const typename Relation<T>::tuple_t& B) {
		for (int i : perm) {
			if (A[i] < B[i])	return true;
			else if (A[i] > B[i])	return false;
		}
		return false; });
}

/*
 * Takes two vectors A and B, and returns a third vector
 * C containing the common elements without repetition
 * 
 * @param A first vector
 * @param B second vector
 * @return a vector containing the common elements in 
 *	A and B
 */
template<typename T>
std::vector<T> common_elems(const std::vector<T>& A,
			      const std::vector<T>& B)
{
	std::unordered_set<T> hsetA{A.begin(), A.end()};
	std::unordered_set<T> hsetB{B.begin(), B.end()};
	std::vector<T> C;
	
	for (auto& i : hsetA) 
		if (hsetB.find(i) != hsetB.end()) 
			C.push_back(i);
	
	std::sort(C.begin(), C.end());

	return C;	
}

/*
 * Takes a vector of integers var_vect and a subvector
 * common_vect, and assign to each number x in var_vect a unique integer
 * p(x) in the range [0, var_vect.size()) such that:
 * - if x appears in common_vect and y does not, then p(x) < p(y)
 * - if both x and y appears in common_vect, then
 *   p(x) < p(y) <=> x < y
 * - x != y => p(x) != p(y)
 * 
 * @param var_vect vector of integers
 * @param common_vect vector of integers, subvector of var_vect
 * 	  WARNING: common_vect must be ordered!!!
 * @return a permutation of [0, var_vect.size()) as described above 
 */
std::vector<int> get_perm(const std::vector<int>& var_vect,
			  const std::vector<int>& common_vect);

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
				 const std::vector<int>& vars2);

/*
 * Performs join operation in two relations.
 *
 * @param rel1 first relation
 * @param rel2 second relation
 * @param var1 tuple of variables for first relation
 * @param var2 tuple of variables for second relation
 * @return result of join operation as a new relation
 */
Relation<int> join(Relation<int>& rel1,
		   Relation<int>& rel2,
		   const std::vector<int>& var1,
     		   const std::vector<int>& var2);


/*
 * Performs join operation for multiple relations.
 *
 * @param relv 			vector containing names of relation files
 * @param varsv 		vector of corresponding variables
 * @param result_vars 	vector to identify variables in the resulting relation
 * @return 				result of join operation as a new relation
 */
Relation<int> multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars);



#endif
