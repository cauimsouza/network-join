#ifndef DEBUG_H
#define DEBUG_H

inline void p(const std::string s)
{
	std::cout<< s<<std::endl;
}

inline void pv(const std::vector<int>& vet)
{
	for (int i : vet) std::cout << i << " ";
	std::cout << std::endl;
}

#endif
