#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <iostream>
#include "ioutil.h"
#include <vector>

template<typename T>
void sort(Relation<T>& rel, const std::vector<int> perm)
{
	std::sort(rel.begin(), rel.end(),
	[perm](const typename Relation<T>::tuple_t& A,
	       const typename Relation<T>::tuple_t& B) {
		for (int i : perm)
			if (A[i] < B[i])	return true;
			else if (A[i] > B[i])	return false;
		return false; });
}

#endif
