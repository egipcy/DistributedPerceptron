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

  std::cout << p.get_rank() << " Election..." << std::endl;
  p.elect_president();
  std::cout << p.get_rank() << " END Election" << std::endl;

  if (p.get_type() == Type::President)
  {
    std::cout << p.get_rank() << " is President" << std::endl;
    p.elect_masters();
    p.init_nn();
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


    std::cout << p.get_rank() << " Received something tag=" << status.MPI_TAG << std::endl;

    int t = status.MPI_TAG;
    if (t == Tag::WeightsMatrix)
    {
      std::cout << p.get_rank() << " Matrix received" << std::endl;

      int count_weight = 0;
      int count_biais = 0;
      // w and b are weights and biases if p is a worker or master
      // but are gradients if p is a president
      MPI_Get_count(&status, MPI_DOUBLE, &count_weight);
      std::vector<double> w(count_weight);
      MPI_Recv(w.data(), count_weight, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
      flag = false;
      while (!flag)
      {
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
      MPI_Get_count(&status, MPI_DOUBLE, &count_biais);
      //std::cout <<"Recev Count_Biais =" <<  count_biais << std::endl;
      std::vector<double> b(count_biais);
      MPI_Recv(b.data(), count_biais, MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD, &status);
      if (p.get_type() == Type::Worker)
      {
        std::cout << p.get_rank() << " Worker receive nn" << std::endl;
        p.set_weights_biases(w, b);

        std::cout << p.get_rank() << " Worker calculate gradients" << std::endl;
        std::pair<std::vector<Matrix>, std::vector<Matrix>> g = p.get_gradients(); // weights, biases

        std::cout << p.get_rank() << " Worker send gradients" << std::endl;
        auto g_weights = serialize(g.first);
        auto g_biases = serialize(g.second);
        //std::cout << "send: " << g_weights.size() << std::endl;
        MPI_Send(g_weights.data(), g_weights.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::WeightsMatrix, MPI_COMM_WORLD);
        //std::cout << "send: " << g_biases.size() << std::endl;
        MPI_Send(g_biases.data(), g_biases.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD);
      }
      else if (p.get_type() == Type::Master)
      {
        std::cout << p.get_rank() << " Master update nn" << std::endl;
        p.set_weights_biases(w, b);
      }
      else // if (p.get_type() == Type::President)
      {
        std::cout << p.get_rank() << " President receive gradients" << std::endl;
        p.update_nn(w, b);

        if (p.has_ended())
        {
          std::cout << p.get_rank() << " END everybody" << std::endl;
          p.end_all(); // send the tag Finished to everybody
        }

        std::cout << p.get_rank() << " President send nn" << std::endl;
        p.send_weights(status.MPI_SOURCE);
      }
    }
    else if (t == Tag::Finished)
    {
      std::cout << p.get_rank() << " END received" << std::endl;
      p.end();
    }
    else if (t == Tag::UpgradeToMaster)
    {
      MPI_Get_count(&status, MPI_DOUBLE, &count);
      std::vector<int> masters(count);
      MPI_Recv(masters.data(), count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
      p.upgrade_to_master(masters);
    }
  }

  std::cout << p.get_rank() << " FINISH" << std::endl;

  MPI_Finalize();
  return 0;
}
