#include "process.hh"

#include <mpi.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

Process::Process(int rank, int w_size, const std::string& filename_data,
  const std::string& filename_parameters, double ratio, int nb_epochs)
  : rank_(rank)
  , w_size_(w_size)
  , type_(Type::Worker)
  , ratio_(ratio)
  , nb_epochs_(nb_epochs)
  , alive_(true)
  , has_ended_(false)
{
  init_datas(filename_data);
  init_parameters(filename_parameters);
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
    v.push_back(datas_.first.columns());
    for (int i = 0; i < parameters_.nb_hidden_layers; i++)
      v.push_back(parameters_.nb_hidden_neurons);
    v.push_back(datas_.second.columns());

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
  int number;
  for (int i = 1; i < w_size_; i++)
    MPI_Send(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
}

void Process::init_datas(const std::string& filename_data)
{
  /*
  ** open file
  ** for each line, split separators (,;)
  ** n-1 first values goes in X (matrix aka vector of vectors)
  ** last value goes in y (matrix aka vector of double)
  ** update datas_
  */

  std::ifstream file_data(filename_data, std::ios::in);
  if(!file_data.is_open())
  {
    std::cerr << "File not found: " << filename_data << std::endl;
    exit(1);
  }

  std::string line;
  std::vector<std::vector<double>> X;
  std::vector<double> Y;
  while (std::getline(file_data, line))
  {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(","));

    std::vector<double> x_double(result.size() - 1);
    for (size_t i = 0; i < x_double.size(); i++)
      x_double[i] = std::stod(result[i]);
    double y_double = std::stod(result[result.size() - 1]);

    X.push_back(x_double);
    Y.push_back(y_double);
  }

  file_data.close();

  datas_ = std::pair<Matrix, Matrix>(Matrix(X), Matrix(Y));
}

void Process::init_parameters(const std::string& filename_parameters)
{
  std::ifstream file_parameters(filename_parameters, std::ios::in);
  if(!file_parameters.is_open())
  {
    std::cerr << "File not found: " << filename_parameters << std::endl;
    exit(1);
  }

  std::string line;

  while (std::getline(file_parameters, line))
  {
    std::string param = line.substr(0, line.find_first_of(" ="));
    std::string value = line.substr(line.find_last_of(" =") + 1);
    if (param == "nb_hidden_layers")
      parameters_.nb_hidden_layers = std::stoi(value);
    else if (param == "nb_hidden_neurons")
      parameters_.nb_hidden_neurons = std::stoi(value);
    else if (param == "learning_rate")
      parameters_.learning_rate = std::stod(value);
    else
      std::cerr << "Unknown parameter: " << param << std::endl;
  }
}