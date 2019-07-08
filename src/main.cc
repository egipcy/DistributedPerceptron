#include <iostream>
#include <ctime>
#include "mpi.h"

#include "process.hh"
#include "matrix/matrix.hh"


int main(int argc, char** argv)
{
  if (argc != 3)
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
      int count_weight;
      int count_biais;
      // w and b are weights and biases if p is a worker or master
      // but are gradients if p is a president
      MPI_Get_count(&status, MPI_DOUBLE, &count_weight);
      std::vector<double> w(count_weight);
      //std::cout << "Recev Count_Weight = " << count_weight << std::endl;
      MPI_Recv(w.data(), count_weight, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);


      MPI_Status status_biais;
      /*MPI_Probe(int source, int tag, MPI_Comm comm,  MPI_Status* status)*/
      // std::cout << "Before the Iprobe to get biais size =" << status.MPI_TAG << std::endl;
      //MPI_Probe(MPI_ANY_SOURCE,Tag::BiasesMatrix,MPI_COMM_WORLD, &status_tmp);
      MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status_biais);
      //std::cout << "after the Iprobe to get biais size =" << status_tmp.MPI_TAG << std::endl;

      MPI_Get_count(&status_biais, MPI_DOUBLE, &count_biais);
      // The get count not receive the good size, need to find the wayt to get the good size;
      // std::cout <<"Recev Count_Biais =" <<  count_biais << std::endl;

      std::vector<double> b(count_biais);
      MPI_Recv(b.data(), count_biais, MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD, &status_biais);

      if(p.get_type() == Type::Worker)
      {
        p.set_weights_biases(w, b);
        std::pair<std::vector<Matrix>, std::vector<Matrix>> g = p.get_gradients(); // weights, biases
        auto g_weights = serialize(g.first);

        auto g_biases = serialize(g.second);
        // std::cout << "send: " << g_weights.size() << std::endl;
        MPI_Send(g_weights.data(), g_weights.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::WeightsMatrix, MPI_COMM_WORLD);
        // std::cout << "send: " << g_biases.size() << std::endl;
        MPI_Send(g_biases.data(), g_biases.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD);
      }
      else if (p.get_type() == Type::Master)
      {
        p.set_weights_biases(w, b);
      }
      else // if (p.get_type() == Type::President)
      {
        p.update_nn(w, b);
        if (p.has_ended())
        {
          p.end_all(); // send the tag Finished to everybody
        }
        p.send_weights(status.MPI_SOURCE);
      }
    }
    else if (t == Tag::Finished)
    {
      p.end();
    }
  }

  MPI_Finalize();
  return 0;
}
