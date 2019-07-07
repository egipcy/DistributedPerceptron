#include <iostream>
#include <ctime>
#include "mpi.h"

#include "header.hh"

#include "process.hh"
#include "matrix/matrix.hh"
#include "reader.cc"




int main(int argc, char** argv) {
  if (argc != __ARGC__) {
    std::cerr << "Wrong number of arguments" << std::endl;
    std::cerr << "Arguments : data's path, master/worker ratio, n_epoch"
    return 1
  }  

  MPI_Init(NULL, NULL);
  int rank, w_size;
  MPI_Comm_size(MPI_COMM_WORLD, &w_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  Process p = Process(rank, w_size, argv[1], argv[2], argv[3]);


}