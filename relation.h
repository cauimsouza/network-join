#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <array>

template<typename T, std::size_t N>
class Relation {
public:
	using tuple_t = std::array<T, N>;
private:
	/* private typenames */
	using tuples_t = typename std::vector<tuple_t>;

	/* member variables */
	const std::size_t arity;
	tuples_t tuples; 

	/* member private functions */
public:
	/* public typenames */
	using iterator = typename tuples_t::iterator;

	/* constructors */	
	explicit Relation();

	/* destructors */

	/* member public funcitons */
	std::size_t size() { return tuples.size(); }
	iterator begin() { return tuples.begin(); }
	iterator end() { return tuples.end(); }

	void push_tuple(tuple_t tpl);
};

template<typename T, std::size_t N>
Relation<T, N>::Relation() : arity(N) 
{}

template<typename T, std::size_t N>
void Relation<T, N>::push_tuple(tuple_t tpl)
{
	tuples.push_back(tpl);		
}

#endif
