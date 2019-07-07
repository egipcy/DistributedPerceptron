#include "process.hh"

Process::Process(int rank, int world_size, std::string filename_data, std::string filename_parameters, double ratio, int nb_epochs)
  : rank_(rank)
  , world_size_(world_size)
  , type_(Type::Worker)
  , ratio_(ratio)
  , nb_epochs_(nb_epochs)
  , alive_(true)
  , has_ended_(false)
{
  std::ifstream file_data(filename_data, ios::in);

  /* TODO:
  ** open file
  ** for each line, split separators (,;)
  ** n-1 first values goes in X (matrix aka vector of vectors)
  ** last value goes in y (matrix aka vector of double)
  ** update datas_
  */
  datas_ = std::pair<Matrix, Matrix>();

  std::ifstream file_parameters(file_parameters, ios::in);
  // TODO
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

bool Process::is_alive() const
{
  return alive_;
}

void Process::set_alive(bool alive)
{
  alive_ = alive;
}

bool Process::has_ended() const
{
  return has_ended_;
}

void Process::elect_president()
{
  // TODO

  if (this->type == Type::President)
  {
    std::vector<int> v;
    v.push_back(datas_.first.size());
    for (int i = 0; i < parameters_.nb_hidden_layers.size(); i++)
      v.push_back(parameters_.nb_hidden_neurons);
    v.push_back(datas_.second.size());

    nn_ = NN(v);
  }
}

void Process::elect_masters()
{
  // TODO
}

void Process::send_ranges()
{
  // TODO
}