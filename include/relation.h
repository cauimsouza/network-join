#ifndef RELATION_H
#define RELATION_H

#include <vector>
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
	std::size_t arity;
	tuples_t tuples; 

	/* member private functions */
public:
	/* public typenames */
	using iterator = typename tuples_t::iterator;

	/* constructors */	
	explicit Relation(std::size_t n) : arity{n} {
		if (n <= 0) throw std::invalid_argument("arity should be\
				positive integer"); }
	Relation(const Relation& rel) : arity{rel.arity}, tuples{rel.tuples} {}
	Relation(Relation&& rel) : arity{rel.arity}, tuples{rel.tuples} {}

	Relation& operator=(Relation&& rel) {
		arity = rel.arity;
		tuples = rel.tuples;
		return *this;
	}

	/* destructors */

	/* member public funcitons */
	std::size_t size() { return tuples.size(); }
	std::size_t get_arity() { return arity; }
	iterator begin() { return tuples.begin(); }
	iterator end() { return tuples.end(); }

	void push_tuple(tuple_t tpl) { tuples.push_back(tpl); }
};

#endif
