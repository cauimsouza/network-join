/*
 * Created in Palaiseau (France) in 30/05/2018 by:
 * Cauim de S. Lima (cauimsouza@gmail.com)
 * Victor Hugo V. Silva (vitorvianna10@gmail.com)
 */

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
