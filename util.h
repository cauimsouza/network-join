#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <iostream>
#include "ioutil.h"

template<typename T, std::size_t N>
void sort(Relation<T, N>& rel, typename Relation<int, N>::tuple_t& perm)
{
	std::sort(rel.begin(), rel.end(),
	[perm](const typename Relation<T, N>::tuple_t& A,
	       const typename Relation<T, N>::tuple_t& B) {
		for (int i : perm)
			if (A[i] < B[i])	return true;
			else if (A[i] > B[i])	return false;
		return false; });
}

#endif
