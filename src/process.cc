#include "header.hh"
#include "process.hh"
#include <mpi.h>
#include <iostream>


Process::Process(int id, int world_size)
  : id_(id)
  , world_size_(world_size)
  , nb_epochs_(__NB_EPOCHS__)
{ }

void Process::start()
{
}

void Process::set_president(int president_id) {
  this->president_id_ = president_id;
}

/**
 * Elects a new president + broadcast president id
 * At the exit of this the function prsident_id_ is set
*/
void Process::elect_president(int world_rank, int world_size)
{
  // President is always 0. The real version should be implemented later
  this->set_president(0);
}