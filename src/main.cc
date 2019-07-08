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
    std::cerr << "Arguments : data's path, config's path";
    return 1;
  }

  MPI_Init(&argc, &argv);
  int rank, world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Process p = Process(rank, world_size, argv[1], argv[2]);
  p.elect_president();

  if (p.get_type() == Type::President)
  {
    p.elect_masters();
    p.send_weights_all();
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
      // w and b are weights and biases if p is a worker or master
      // but are gradients if p is a president
      MPI_Get_count(&status, MPI_DOUBLE, &count);
      std::vector<double> w(count);
      MPI_Recv(w.data(), count, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
      std::vector<double> b(count);
      MPI_Recv(b.data(), count, MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD, &status);

      if(p.get_type() == Type::Worker)
      {
        p.set_weights_biases(w, b);
        std::pair<std::vector<Matrix>, std::vector<Matrix>> g = p.get_gradients(); //weights, biases
        auto g_weights = serialize(g.first);
        auto g_biases = serialize(g.second);
        MPI_Send(g_weights.data(), g_weights.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::WeightsMatrix, MPI_COMM_WORLD);
        MPI_Send(g_biases.data(), g_biases.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD);
      }
      else if (p.get_type() == Type::Master)
      {
        p.set_weights_biases(w, b);
      }
      else // if (p.get_type() == Type::President)
      {
        bool ended = p.update_nn(w, b);
        if (ended)
        {
          p.end_all(); //send the tag Finished to everybody
          std::cout << "Ended with success" << std::endl;
        }
        std::cout << "s" << std::endl;
        p.send_weights(status.MPI_SOURCE);
      }
    }
    else if (t == Tag::Finished)
    {
      p.end();
    }
  }
}