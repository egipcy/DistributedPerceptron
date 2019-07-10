#include <iostream>
#include <ctime>
#include <cassert>
#include "mpi.h"
#include <sstream>
#include "timeout.hh"
#include "process.hh"
#include "matrix/matrix.hh"
#include "timeout.hh"

int main(int argc, char** argv)
{
  std::vector<double> kill_times = {2.0, 3.7};
  std::vector<double> kill_ids = {9, 8};

  int debug = 0;
  if (argc < 4)
  {
    std::cerr << "Wrong number of arguments" << std::endl;
    std::cerr << "Arguments : data's path, config's path, neural network's path" << std::endl;
    return 1;
  }

  MPI_Init(&argc, &argv);
  int rank, world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Process p = Process(rank, world_size, argv[1], argv[2]);
  if(argc == 5)
    p.set_need_load();
  p.elect_president();
  if (p.get_type() == Type::President)
  {
    p.elect_masters();
    p.init_nn();
    if(p.get_need_load())
      p.load_nn(argv[4]);
    p.send_weights_all();
  }

  std::vector<std::function<bool()>> kill_timeouts(kill_times.size());
  for (size_t i = 0; i < kill_timeouts.size(); i++)
    kill_timeouts[i] = generate_timer(kill_times[i]);

  MPI_Status status;
  int count;
  int nb_save = 0;
  int nb_pass = 0;
  while (!p.has_ended())
  {
    for (size_t i = 0; i < kill_timeouts.size(); i++)
      if (!kill_timeouts[i]())
      {
        if (kill_ids[i] == p.get_rank())
        {
          std::cout << p.get_rank() << " gets killed" << std::endl;
          MPI_Finalize();
          return 0;
        }
        kill_timeouts.erase(kill_timeouts.begin() + i);
        kill_times.erase(kill_times.begin() + i);
        kill_ids.erase(kill_ids.begin() + i);
        i--;
      }

    int flag = false;
    Type ptype = p.get_type();
    if (ptype == Type::President || ptype == Type::Worker)
    {
      while (!flag)
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
    }
    else // p.get_type() == Type::Master
    {
      auto timer = generate_timer(_MASTER_WAIT_TIMEOUT_);
      while (!flag)
      {
        if (!timer())
        {
          std::cout << p.get_rank() << " says president is dead" << std::endl;
          p.master_to_president();
          if (p.get_type() == Type::President)
          {
            std::cout << "master " << p.get_rank() << " is the new president. " << std::endl;
            p.send_weights_all();
            status.MPI_TAG = -1;
            break;
          }
          else
          {
            timer = generate_timer(_MASTER_WAIT_TIMEOUT_);
          }
        }
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      }
    }
    int t = status.MPI_TAG;
    if (t == Tag::WeightsMatrix)
    {
      if (p.get_type() != Type::President)
      {
        if (status.MPI_SOURCE != p.get_president() && status.MPI_SOURCE != -1)
        {
          std::cout << p.get_rank() << "president was " << p.get_president() << " but now it's "
          << status.MPI_SOURCE << std::endl;
          p.set_president(status.MPI_SOURCE);
        }
      }
      int count_weight = 0;
      int count_biais = 0;
      // w and b are weights and biases if p is a worker or master
      // but are gradients if p is a president
      MPI_Get_count(&status, MPI_DOUBLE, &count_weight);
      std::vector<double> w(count_weight);

      MPI_Recv(w.data(), count_weight, MPI_DOUBLE, status.MPI_SOURCE, Tag::WeightsMatrix, MPI_COMM_WORLD, &status);

      flag = false;
      while (!flag)
        MPI_Iprobe(status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD, &flag, &status);

      MPI_Get_count(&status, MPI_DOUBLE, &count_biais);

      std::vector<double> b(count_biais);

      MPI_Recv(b.data(), count_biais, MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD, &status);

      if (p.get_type() == Type::Worker)
      {
        p.set_weights_biases(w, b);

        std::pair<std::vector<Matrix>, std::vector<Matrix>> g = p.get_gradients(); // weights, biases

        auto g_weights = serialize(g.first);
        auto g_biases = serialize(g.second);
        MPI_Send(g_weights.data(), g_weights.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::WeightsMatrix, MPI_COMM_WORLD);
        MPI_Send(g_biases.data(), g_biases.size(), MPI_DOUBLE, status.MPI_SOURCE, Tag::BiasesMatrix, MPI_COMM_WORLD);
      }
      else if (p.get_type() == Type::Master)
      {
        nb_pass++;
        p.set_weights_biases(w, b);
        //std::cout << "Master write the saving file" <<  std::endl;
        std::stringstream filename;
        std::cout << "Master n°" << p.get_rank() << " SAVE #" << nb_pass << std::endl;
        filename << "Save_master_" << p.get_rank();
        p.save_nn(filename.str(),p.get_time_to_save() * nb_pass);
      }
      else // if (p.get_type() == Type::President)
      {
        p.update_nn(w, b);

        if (p.has_ended())
        {
          p.save_nn(argv[3], p.get_epoch());
          p.end_all(); // send the tag Finished to everybody
        }

        p.send_weights(status.MPI_SOURCE);

        if (p.get_epoch() % p.get_time_to_save() == 0)
        {
          nb_save++;
          p.send_weights_to_master();
        }
      }
    }
    else if (t == Tag::Finished)
    {
      p.end();
    }
    else if (t == Tag::UpgradeToMaster)
    {
      MPI_Get_count(&status, MPI_INT, &count);
      std::vector<int> masters(count);
      MPI_Recv(masters.data(), count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
      p.upgrade_to_master(masters);
    }
  }

  std::cout << p.get_rank() << " FINISH" << std::endl;

  MPI_Finalize();
  return 0;
}
