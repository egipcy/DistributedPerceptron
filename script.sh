mpic++ src/main.cc src/process.cc &&
mpirun -np 5 -hostfile hostfile ./a.out