#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>
#include <functional>
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
  //   open file
  std::ifstream infile(filename);
  std::string line;
  std::vector<double> X;
  std::vector<double> Y;
  int row = 0;
  int cols = 0;

  // for each line, split separators (,;)
  if(!infile.is_open())
    exit(1);
  while(std::getline(infile, line))
  {
    std::stringstream iss(line);
    std::string values;
    while(std::getline(iss,values,';'))
    {
      if (row < 1)
        cols++;
      X.push_back(std::stod(values));
    }
    row++;
    Y.push_back(X[cols*row-row]);
    X.erase(X.begin()+(row*cols)-row); 
  }
  infile.close();

  // Create the vector  Y
  /*Move the N elt of each lines from  the matrix X to the vector Y  */
  cols = cols-1;
  for(int i  = 0; i < X.size(); i++)
  {
    if(i % cols == 0)
      std::cout << std::endl;
    std::cout << X[i] ;
  }
  std::cout << std::endl;

  return  std::make_pair(Matrix(row,cols,X), Y);
}
 /*
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
*/
void mpi_init_failed()
{
  std::cout << "\n";
  std::cout << "HELLO_MPI - Fatal error!\n";
  std::cout << "  MPI_Init returned nonzero ierr.\n";
  exit(1);
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
