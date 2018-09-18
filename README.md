# Distributed Join on Social Network Data

Application to perform join operations using different distributed algorithms. In particular, can be used to detect subgraphs in social network graphs.

## Authors

Cauim de Souza Lima  
Victor Hugo Vianna

## Requirements

OpenMPI  
Boost lib

## Build

If you're running the code from Polytechnique's cluster, set SALLES_DINFO=true in the Makefile (be careful with the blank spaces), otherwise set SALLES_DINFO=false.
Then open a shell and run "make" from the root directory. This will prompt the creation of two files: bin/test_join and bin/test_triangles

## Testing join

tests/join/relations should contain all the relation files you want to join, in the usual format. Ex:  
  
tests/join/relations/relation1.txt  
10 0  
10 1  
  
tests/join/inputs should contain a file with the join information. Such file must start with the number N of relations you want to join, followed by N lines. Each line must have the arity of the relation, the name of its file and the variable indices. Ex:  
  
tests/join/input/test1.txt  
2  
2 relation1.txt 0 1  
2 relation1.txt 0 2  
  
In the example above, the first line means that we are going to join 2 relations. The first has an arity of 2, it's contained in "tests/join/inputs/relation1.txt" and the variables are (x<sub>0</sub>, x<sub>1</sub>). Similarly, the second one has arity 2, the same file will be used, but now its variables are (x<sub>0</sub>, x<sub>2</sub>).  
If you want, you can add a verification file to tests/join/output with the SAME NAME as your input. This file will be compared to the output of the program. In this case we could have  
  
tests/join/output/test1.txt  
10 0 0  
10 0 1  
10 1 0  
10 1 1  
  
To run the program, open a shell from the ROOT DIRECTORY and type in  

```
salloc -n <# of processes> mpirun -x LD_LIBRARY_PATH=./lib bin/test_join test1.txt <algorithm> <hash>
```	

(If you're not running from Polytechnique, remove the "-x LD_LIBRARY_PATH=./lib")  
&lt;algorithm&gt; option can be (without quotes): "sequential", "normal_distrib", "optimized_distrib", "hypercube"  
&lt;hash&gt; option can be (without quotes): "mod_hash", "mult_hash", "murmur_hash" (if no hash is provided, mod_hash will be used)  
  
## Testing triangles

Same as above, except that the input to the program is just a single relation file tests/triangles/inputs/input1.txt where we will detect every possible triangle. Ex:  
  
tests/triangles/inputs/input1.txt  
0 1  
1 2  
2 0  
  
Should output  
0 1 2  
1 2 0  
2 0 1  
