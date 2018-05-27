#include <fstream>
#include <sstream>
#include <iostream>
#include "relation.h"
#include "ioutil.h"

std::istream& operator>>(std::istream& is, Relation<int>& rel)
{
	auto arity = rel.get_arity();
	std::size_t mod = 0;
	typename Relation<int>::tuple_t tpl(arity);
	for (int var; is >> var;) {
		tpl[mod] = var;
		mod++;
		if (mod == arity) {
			mod = 0;
			rel.push_tuple(tpl);
		}
	}

	return is;
}

std::ostream& operator<<(std::ostream& os, Relation<int>& rel)
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

std::size_t read_arity(std::string& filename)
{
	std::ifstream ifs(filename);
	std::string first_line;

	std::getline(ifs, first_line);
	std::istringstream line_stream(first_line);

	int arity = 0;
	for (int x; line_stream >> x;)	arity++;

	ifs.close();
	return arity;
}

void read_relation(std::string& filename, Relation<int>& rel)
{
	std::ifstream ifs(filename);
	ifs >> rel;
	ifs.close();
}

void write_relation(const std::string& filename, Relation<int>& rel)
{
	std::ofstream ofs(filename);
	ofs << rel;
	ofs.close();
}

