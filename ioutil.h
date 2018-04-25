#ifndef	IOUTIL_H
#define IOUTIL_H

#include "relation.h"
#include <fstream>

template<std::size_t N>
std::istream& operator>>(std::istream& is, Relation<int, N>& rel)
{
	int var;
	std::size_t mod = 0;
	typename Relation<int, N>::tuple_t tpl;
	while (is >> var) {
		tpl[mod] = var;
		mod++;
		if (mod == N) {
			mod = 0;
			rel.push_tuple(tpl);
		}
	}

	return is;
}

template<std::size_t N>
std::ostream& operator<<(std::ostream& os, Relation<int, N>& rel)
{
	for (auto& tuple : rel) {
		bool first = true;
		for (auto& el : tuple) {	
			if (!first)	os << " ";
			os << el;
			first = false;
		}
		os << std::endl;
	}

	return os;
}

template<std::size_t N>
void read_file(std::string& filename, Relation<int, N>& rel)
{
	std::ifstream ifs(filename);
	ifs >> rel;
	ifs.close();
}

template<std::size_t N>
void write_file(const std::string& filename, Relation<int, N>& rel)
{
	std::ofstream ofs(filename);
	ofs << rel;
	ofs.close();
}

#endif
