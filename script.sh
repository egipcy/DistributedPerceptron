mpic++ -std=c++17 src/*.cc src/matrix/*.cc src/nn/*.cc &&
mpirun -np 2 -hostfile hostfile ./a.out train.csv CONFIG
