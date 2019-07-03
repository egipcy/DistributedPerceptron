#include "header.hh"
#include "process.hh"


Process::Process(int id, int world_size, int nb_epoch = NB_EPOCHS)
  : id_(id)
  , world_size_(world_size)
  , nb_epochs_(nb_epoch)
{ }

void Process::start()
{
}

void Process::elect_predisent()
{
  
}