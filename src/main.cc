#include <iostream>
#include "mpi.h"

#include "process.hh"

int print_usage_and_exit()
{
  std::cout << "Usage: ./bin FILE NBMASTERS NBWORKERS NBEPOCHS" << std::endl;

  MPI_Finalize();

  return 0; //WHY 0 ?
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
  std::vector<Worker> workers;
  for (auto i = 0; i < nb_masters; i++)
    masters.emplace_back(ids_masters[i], ids_masters, ids_workers, nb_epochs, datas);
  for (auto i = 0; i < nb_workers; i++)
    workers.emplace_back(ids_workers[i], ids_masters, ids_workers);

  return std::pair<std::vector<Master>, std::vector<Worker>>(masters, workers);
}

void mpi_init_failed()
{
  std::cout << "\n";
  std::cout << "HELLO_MPI - Fatal error!\n";
  std::cout << "  MPI_Init returned nonzero ierr.\n";
  exit(1);
}

int main2(int argc, char** argv)
{
  int ierr = MPI_Init(&argc, &argv);
  if (ierr != 0)
  {
    mpi_init_failed();
  }
  //TEST
  // Parsing arguments
  if (argc != 5)
    return print_usage_and_exit();
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  //datas = read_file(std::string(argv[1]));

  /**
   * Basic version
   * nb_masters should depend on world_size
  */
  int nb_masters = 1;
  int nb_workers = world_size - nb_masters;
  int nb_epochs = 70;

  /** We should exit
  if (nb_workers == 0)
  {

  }
  */

  auto pair = build_masters_workers(nb_masters, nb_workers, nb_epochs, datas);
  auto masters = pair.first;
  auto workers = pair.second;

  // Start everybody
  // send message tag 1
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

int main(int argc, char** argv)
{
  //Création 1 Worker
  //Election President
  //Recv
  int ierr = MPI_Init(&argc, &argv);
  if (ierr != 0)
  {
    mpi_init_failed();
  }

  if (argc != 5)
    return print_usage_and_exit();
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  Process* process = new Process(world_rank, world_size);
  int flag = 0;
  process->elect_president();

/*After Election President */
  while(flag != -1)
  {
    MPI_Status status;
    int data;
    MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE,  MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  
    switch (status.MPI_TAG)
    {   
        case 0:
        std::cout << world_rank << " " <<status.MPI_SOURCE  << std::endl;    
        break;
        case -1:
          flag = status.MPI_TAG;
          break;
        case 1:
          flag = status.MPI_TAG;
          break;
          /* Process become worker */
        default:
          std::cout << "STRANGE TAG: " << status.MPI_TAG << std::endl;
          break;
    }
    flag = status.MPI_TAG;
  }
 //datas = read_file(std::string(argv[1]));

  MPI_Finalize();
  return 0;
}