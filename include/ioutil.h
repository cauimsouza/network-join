#ifndef	IOUTIL_H
#define IOUTIL_H

#include "relation.h"
#include <cstddef>

std::istream& operator>>(std::istream& is, Relation<int>& rel);

std::ostream& operator<<(std::ostream& os, Relation<int>& rel);

std::size_t read_arity(std::string& filename);

void read_file(std::string& filename, Relation<int>& rel);

void write_file(const std::string& filename, Relation<int>& rel);

#endif
