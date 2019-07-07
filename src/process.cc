#include "process.hh"

Process::Process(int rank, int world_size, std::string filename, double ratio, int nb_epochs);
  : rank_(rank)
  , world_size_(world_size)
  , type_(Type::Worker)
  , ratio_(ratio)
  , nb_epochs_(nb_epochs)
{
  std::ifstream file(filename, ios::in);

  /* TODO:
  ** open file
  ** for each line, split separators (,;)
  ** n-1 first values goes in X (matrix aka vector of vectors)
  ** last value goes in y (matrix aka vector of double)
  ** update datas_
  */
  datas_ = std::pair<Matrix, Matrix>();
}

int Process::get_rank() const
{
  return rank_;
}

Type Process::get_type() const
{
  return type_;
}

void Process::set_type(Type type)
{
  type_ = type;
}

NN& Process::get_nn()
{
  return nn_;
}

void Process::elect_president()
{
  // TODO
}