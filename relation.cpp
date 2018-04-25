#include "relation.h"
#include <istream>

std::istream& operator>>(std::istream& is, Relation& rel)
{
	std::size_t arity = rel.arity();
	std::array<int, arity> arr;

	for (std::size_t i = 0; i < arity; i++) 
		is >> arr[i];
	
	rel.push_array(arr);

	return is;
}

template<std::size_t N>
void Relation::push_array(std::array<int, N> arr)
{
	tuples.push_back(arr);		
}
