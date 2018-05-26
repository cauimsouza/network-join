namespace mpi = boost::mpi;

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
 * @param relv vector of relations
 * @param varsv vector of corresponding variables
 * @param result_vars vector to identify variables in the resulting relation
 * @return result of join operation as a new relation
 */
Relation<int> distributed_multiway_join(std::vector<Relation<int>>& relv,
		   std::vector<std::vector<int>>& varsv,
		   std::vector<int>& result_vars);