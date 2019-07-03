#include "header.hh"
#include "process.hh"
#include <mpi.h>


Process::Process(int id, int world_size, int nb_epoch = NB_EPOCHS)
  : id_(id)
  , world_size_(world_size)
  , nb_epochs_(nb_epoch)
{ }

void Process::start()
{
}

void Process::elect_president()
{
  int id = this->id_;
  int prevId = id - 1 < 0 ? this->world_size_ - 1 : id - 1;
  int nextId = id + 1 >= this->world_size_ ? 0 : this->world_size_ + 1;
  MPI_Send(&id, 1, MPI_INT, prevId, 0, MPI_COMM_WORLD);
  MPI_Send(&id, 1, MPI_INT, nextId, 0, MPI_COMM_WORLD);
}