#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char** argv)
{
  int rank, size, len;
  char version[MPI_MAX_LIBRARY_VERSION_STRING];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_library_version(version, &len);

  std::vector<std::vector<MPI_Request>> req(size);
  for (auto i = 1; i < size; i++)
    req[i] = std::vector<MPI_Request>(10);

  MPI_Status status;
  MPI_Request r;
  int tag = 0;
  for (auto i = 1; i < size; i++)
    if (rank == i)
      for (auto k = 0; k < 10; k++)
      {
        MPI_Issend(&k, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &r);
        std::cout << "Send " << k << " from " << i << " to " << 0 << std::endl;
      }


  int data = 42;
  std::vector<int> v;
  for (auto i = 0; i < size; i++)
    v.push_back(-1);

  if (rank == 0)
    for (auto i = 1; i < size; i++)
      for (auto k = 0; k < 10; k++)
      {
        MPI_Recv(&data, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
        if (v[i]++ + 1 != data)
          std::cout << "ERROR: " << v[i] << " >= " << data << " from " << i << std::endl;
        std::cout << "Recv " << data << " from " << i << std::endl;
      }

  MPI_Finalize();
}