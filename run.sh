export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
mpirun -x LD_LIBRARY_PATH -np 4 bin/test_join cycle.txt 1