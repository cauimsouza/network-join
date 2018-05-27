#ifndef MPIUTIL_H
#define MPIUTIL_H

#include <boost/mpi.hpp> 
#include "relation.h"

namespace mpi = boost::mpi;
namespace constants{
	const int NONE = -1; // signalize when there are no common variables
	const int ROOT = 0; // signalize when there are no common variables
}

/* Performs join operation in a distributed fashion
 * using Boost's MPI interface.
 *
 * @param rel1 first relation
 * @param rel2 second relation
 * @param vars1 tuple of variables for first relation
 * @param vars2 tuple of variables for second relation
 * @return result of join operation
 */
Relation<int> distributed_join(Relation<int> &rel1,
		      Relation<int> &rel2,
		      std::vector<int> &vars1,
		      std::vector<int> &vars2);

/*
 * Performs join operation for multiple relations
 * in a distributed fashion using Boost's MPI
 * implementation
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @param forward flag to enable auto-forward optimization
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars, bool forward=true);

/*
 * Performs join operation for multiple relations
 * in a distributed fashion using Boost's MPI
 * implementation, by applying the hypercube algorithm
 *
 * @param rel_namesv vector containing relations' filenames
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @param forward flag to enable auto-forward optimization
 * @return result of join operation as a new relation
 */
Relation<int> hypercube_distributed_multiway_join(std::vector<std::string>& rel_namesv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars);

#endif
