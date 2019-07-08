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
  int rank, w_size;
  MPI_Comm_size(MPI_COMM_WORLD, &w_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Process p = Process(rank, w_size, argv[1], argv[2], std::stod(argv[3]), std::stoi(argv[4]));
  p.elect_president();

  if (p.get_type() == Type::President)
  {
    p.elect_masters();
    p.send_ranges();
  }
  
  MPI_Status status;
  int number_amount;
  while (!p.has_ended())
  {
    MPI_Status status;
    int flag = false;
    int recv_number = 0;

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
      else
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
    }

    MPI_Recv(&recv_number,1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
    int number = 1;

    switch(status.MPI_TAG)
    {
      case 1:
        std::cout << rank << std::endl;
        MPI_Send(&number, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        break;
      case 2:
        std::cout << "President recieved a response from "
          << status.MPI_SOURCE << std::endl;
        break;
      default:
        std::cout << "Tag doesn\'t match: " << status.MPI_TAG << " " << status.MPI_SOURCE << std::endl;
    }
  }
}