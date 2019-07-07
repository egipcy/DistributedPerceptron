#include "process.hh"

#include <boost/algorithm/string.hpp>

Process::Process(int rank, int world_size, std::string filename_data, std::string filename_parameters, double ratio, int nb_epochs)
  : rank_(rank)
  , world_size_(world_size)
  , type_(Type::Worker)
  , ratio_(ratio)
  , nb_epochs_(nb_epochs)
  , alive_(true)
  , has_ended_(false)
{
  /*
  ** open file
  ** for each line, split separators (,;)
  ** n-1 first values goes in X (matrix aka vector of vectors)
  ** last value goes in y (matrix aka vector of double)
  ** update datas_
  */

  std::ifstream file_data(filename_data, ios::in);
  if(!file_data.is_open())
    exit(1);

  std::string line;
  std::vector<std::vector<double>> X;
  std::vector<double> Y;
  while (std::getline(file_data, line))
  {
    std::vector<string> result;
    boost::split(result, line, boost::is_any_of(";"));

    std::vector<double> x_double(result.size() - 1);
    for (size_t i = 0; i < v_to_double.size() - 1; i++)
      x_double[i] = std::stod(result[i]);
    double y_double = std::stod(result[result.size() - 1]);

    X.push_back(x_double);
    Y.push_back(y_double);
  }

  file_data.close();

  datas_ = std::pair<Matrix, Matrix>(Matrix(X), Matrix(Y));


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
  //TODO
  // President election
  if (rank_ == 0)
  {
    type_ == Type::President;
  }
  president_id_ = 0;


  if (type_ == Type::President)
  {
    std::vector<int> v;
    v.push_back(datas_.first.size());
    for (int i = 0; i < parameters_.nb_hidden_layers.size(); i++)
      v.push_back(parameters_.nb_hidden_neurons);
    v.push_back(datas_.second.size());

    nn_ = NN(v);
  }
  president_id_ = 0
}

void Process::elect_masters()
{
  // TODO
}

void Process::send_ranges()
{
  // TODO
  int number;
  for (int i = 1; i < w_size_; i++)
    MPI_Send(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
}