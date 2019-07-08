#include <iostream>
#include <ctime>
#include "mpi.h"

#include "header.hh"

#include "process.hh"
#include "matrix/matrix.hh"


int main(int argc, char** argv)
{
  if (argc != __ARGC__)
  {
    std::cerr << "Wrong number of arguments" << std::endl;
    std::cerr << "Arguments : data's path, config's path, master/worker ratio, n_epoch";
    return 1;
  }

  MPI_Init(&argc, &argv);
  int rank, world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Process p = Process(rank, world_size, argv[1], argv[2], std::stod(argv[3]), std::stoi(argv[4]));
  p.elect_president();

  if (p.get_type() == Type::President)
  {
    p.elect_masters();
  }
  
  MPI_Status status;
  int count;
  while (!p.has_ended())
  {
    int flag = false;
    while (!flag)
    {
      if (p.get_type() == Type::President)
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
      else if (p.get_type() == Type::Worker)
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
      else // p.get_type() == Type::Master
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
    }

    int t = status.MPI_TAG;
    if (t == Tag::WeightsMatrix)
    {
      MPI_Get_count(&status, MPI_DOUBLE, &count);
      std::vector<double> w(count);
      MPI_Recv(w.data(), count, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
      p.set_weights(w);
      break;
    }
  }
}