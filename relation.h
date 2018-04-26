#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <array>
#include <stdexcept>

template<typename T>
class Relation {
public:
	using tuple_t = std::vector<T>;
	using value_type = T;
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
	explicit Relation(std::size_t n) : arity{n} {
		if (n <= 0) throw std::invalid_argument("arity should be\
				positive integer"); }

	/* destructors */

	/* member public funcitons */
	std::size_t size() { return tuples.size(); }
	std::size_t get_arity() { return arity; }
	iterator begin() { return tuples.begin(); }
	iterator end() { return tuples.end(); }

	void push_tuple(tuple_t tpl);
};

template<typename T>
void Relation<T>::push_tuple(tuple_t tpl)
{
	if (tpl.size() != arity)
		throw std::invalid_argument("tpl size does not match arity");

	tuples.push_back(tpl);		
}

#endif
