mpic++ src/*.cc &&
mpirun -np 5 -hostfile hostfile ./a.out