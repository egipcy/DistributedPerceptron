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
  p.elect_president();
  if (p.get_type() == Type::President)
  {

  }
  
  MPI_Status status;
  int number_amount, flag;
  while (!p.has_ended())
  {
    if (p.get_type() == Type::President)
    {
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    }
    else if (p.get_type() == Type::Worker)
    {
      
    }
    else {

    }
    


  }
}