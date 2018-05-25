#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <stdexcept>
#include <boost/serialization/vector.hpp>

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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		ar & tuples;
		ar & arity;
	}
public:
	/* public typenames */
	using iterator = typename tuples_t::iterator;

	/* constructors */	
	Relation(std::size_t n = 1) : arity{n} {
		if (n <= 0) throw std::invalid_argument("arity should be\
				positive integer"); }

	/* copy constructor */
	Relation(const Relation &rel) : arity{rel.arity}, tuples{rel.tuples} {}

	/* move constructor */
	Relation(Relation &&rel) : arity{rel.arity}, tuples{std::move(rel.tuples)} {rel.arity = 0; rel.tuples.clear();}

	/* copy assignment */
	Relation& operator=(const Relation &rel);

	/* move assignment */
	Relation& operator=(Relation &&rel);
	/* destructors */

	/* member public functions */
	std::size_t size() { return tuples.size(); }
	std::size_t get_arity() { return arity; }
	iterator begin() { return tuples.begin(); }
	iterator end() { return tuples.end(); }

	void push_tuple(tuple_t tpl) { tuples.push_back(tpl); }
};

/* copy assignment */
template<typename T>
Relation<T>& Relation<T>::operator=(const Relation &rel) {
	if (this == &rel)
		return *this;

	arity = rel.arity;
	tuples = rel.tuples;

	return *this;
}

/* move assignment */
template<typename T>
Relation<T>& Relation<T>::operator=(Relation &&rel) {
	if (this == &rel)
		return *this;

	arity = rel.arity;
	tuples = std::move(rel.tuples);

	rel.arity = 0;
	rel.tuples.clear();

	return *this;
}

#endif
