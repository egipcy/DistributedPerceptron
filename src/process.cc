#include "process.hh"

#include <mpi.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

Process::Process(int rank, int world_size, const std::string& filename_data,
  const std::string& filename_parameters)
  : rank_(rank)
  , world_size_(world_size)
  , type_(Type::Worker)
  , i_epoch_(0)
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

void Process::save_nn(const std::string& filename) const
{
  return nn_.save(filename);
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

void Process::end()
{
  has_ended_ = true;
}

void Process::end_all() const
{
  int n = 0;
  for (auto w: workers_)
    MPI_Send(&n, 1, MPI_INT, w, Tag::Finished, MPI_COMM_WORLD);
  for (auto m: masters_)
    MPI_Send(&n, 1, MPI_INT, m, Tag::Finished, MPI_COMM_WORLD);
}


void send_to_neighbours(int tag, int rank, int id, int world_size)
{
  int destright = (rank + 1) % world_size;
  MPI_Send(&id,1, MPI_INT,destright,tag,MPI_COMM_WORLD);
}

void Process::elect_president()
{
  /*
  std::cout << "Begin President" << std::endl;
  int tag = Tag::Election;
  int id = rank_;
  while (tag == Tag::Election)
  {
    send_to_neighbours(tag,rank_,id,world_size_);
    MPI_Status status;
    int get_id = rank_;
    MPI_Recv(&get_id,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG ,MPI_COMM_WORLD,&status);
    if (status.MPI_TAG == Tag::Endelection || status.MPI_TAG == Tag::Election)
    {
      if (get_id == rank_) //I'm the president
      { 
        tag = Tag::Endelection;
        president_id_ = get_id; 
        send_to_neighbours(tag,rank_,rank_,world_size_);
        break;
      }
      if (status.MPI_TAG == Tag::Endelection) //I'm worker
      {
        tag = Tag::Endelection;
        president_id_ = get_id;
      }
      if (get_id > id) //No one was choosen
        id = get_id;   
    }
  } 
  */

  // President election
  president_id_ = 0;
  if (rank_ == president_id_)
    type_ = Type::President;
}

void Process::elect_masters()
{
  // TODO
  // Fill masters_ and workers_ with their ranks and send type to masters
  for (int i = 0; i < world_size_; i++)
  {
    if (i == president_id_)
      continue;
    workers_.push_back(i);
  }
}

void Process::init_nn()
{
  // Init neural network
  std::vector<int> v;
  v.push_back(datas_.first.columns());
  for (int i = 0; i < parameters_.nb_hidden_layers; i++)
    v.push_back(parameters_.nb_hidden_neurons);
  v.push_back(datas_.second.columns());
  nn_ = NN(v);
}

void Process::init_datas(const std::string& filename_data)
{
  /*
  ** open file
  ** for each line, split separators (,)
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
    else if (param == "nb_epochs")
      parameters_.nb_epochs = std::stoi(value);
    else if (param == "ratio")
      parameters_.ratio = std::stod(value);
    else
      std::cerr << "Unknown parameter: " << param << std::endl;
  }
}

void Process::send_weights(int dest)
{
 
  std::vector<double> weights = serialize(nn_.get_weights());
  
  MPI_Send(weights.data(), weights.size(), MPI_DOUBLE, dest, Tag::WeightsMatrix, MPI_COMM_WORLD);
 
  std::vector<double> biases = serialize(nn_.get_biases());
  
  MPI_Send(biases.data(), biases.size(), MPI_DOUBLE, dest, Tag::BiasesMatrix, MPI_COMM_WORLD);
}

void Process::send_weights_all()
{
  for (auto& w: workers_)
    send_weights(w);
}

void Process::set_weights_biases(const std::vector<double>& weights, const std::vector<double>& biases)
{
  nn_ = NN(deserialize(weights), deserialize(biases));
}

std::pair<std::vector<Matrix>, std::vector<Matrix>> Process::get_gradients()
{
  return nn_.backpropagation(datas_.first, datas_.second);
}

void Process::update_nn(const std::vector<double>& gradients_w, const std::vector<double>& gradients_b)
{
  nn_.update_simple(deserialize(gradients_w), deserialize(gradients_b), parameters_.learning_rate);
  if (++i_epoch_ == parameters_.nb_epochs)
    has_ended_ = true;
}
