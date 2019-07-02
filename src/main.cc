#include <iostream>

#include "master.hh"
#include "worker.hh"

int print_usage_and_exit()
{
  std::cout << "Usage: ./bin FILE NBMASTERS NBWORKERS NBEPOCHS" << std::endl;

  MPI_Finalize();

  return 0;
}

std::pair<Matrix, std::vector<double>>
read_file(std::string filename)
{
  /* TODO:
  ** open file
  ** for each line, split separators (,;)
  ** n-1 first values goes in X (matrix aka vector of vectors)
  ** last value goes in y (vector)
  */

  return std::pair<Matrix, std::vector<double>>();
}

std::pair<std::vector<Master>, std::vector<Worker>>
build_masters_workers(int nb_masters, int nb_workers, int nb_epochs,
  std::pair<Matrix, std::vector<double>> datas)
{
  // Init ids
  std::vector<int> ids_masters;
  std::vector<int> ids_workers;

  for (auto i = 0; i < nb_masters; i++)
    ids_masters.push_back(i);
  for (auto i = 0; i < nb_workers; i++)
    ids_workers.push_back(i + nb_masters);

  // Init masters and workers
  std::vector<Master> masters;
  std::vector<Workers> workers;
  for (auto i = 0; i < nb_masters; i++)
    masters.emplace_back(ids_masters[i], ids_masters, ids_workers, nb_epochs, datas);
  for (auto i = 0; i < nb_workers; i++)
    workers.emplace_back(ids_workers[i], ids_masters, ids_workers);

  return std::pair<>(masters, workers);
}

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  //TEST
  // Parsing arguments
  if (argc != 5)
    return print_usage_and_exit();

  datas = read_file(std::string(argv[1]));
  /* TODO
  int nb_masters = ...;
  int nb_workers = ...;
  int nb_epochs = ...;
  */

  auto pair = build_masters_workers(nb_masters, nb_workers, nb_epochs, datas);
  auto masters = pair.first;
  auto workers = pair.second;

  // Start everybody
  for (auto& m: masters)
    m.start();
  for (auto& w: workers)
    w.start();

  bool loop = true;
  while (loop)
  {
    for (auto& m: masters)
      if (!m.run())
      {
        loop = false;
        break;
      }
      
    for (auto& w: workers)
      w.run();

    // Killing loop
    /* TODO
    if (key pressed)
    {
      int number;
      std::cin >> number;
      if (number < masters.count())
      {
        if (masters[number].is_awake())
          masters[number].sleep();
        else
          masters[number].awake();
      }
      else
      {
        if (workers[number - masters.count()].is_awake())
          workers[number - masters.count()].sleep();
        else
          workers[number - masters.count()].awake();
      }
    }
    */
  }

  MPI_Finalize();

  return 0;
}

