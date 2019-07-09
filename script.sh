mpic++ -std=c++17 src/main.cc src/process.cc src/matrix/*.cc src/nn/*.cc &&
mpirun -np 2 -hostfile hostfile ./a.out train.csv CONFIG NN.nn
