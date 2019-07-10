#include "process.hh"

#include <mpi.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include "timeout.hh"

Process::Process(int rank, int world_size, const std::string& filename_data,
  const std::string& filename_parameters)
  : rank_(rank)
  , world_size_(world_size)
  , right_id_((rank_ + 1) % world_size_)
  , president_id_(rank)
  , type_(Type::Worker)
  , i_epoch_(0)
  , alive_(true)
  , has_ended_(false)
{
  left_id_ = rank - 1 < 0 ? world_size_ - 1 : rank - 1;
  init_datas(filename_data);
  init_parameters(filename_parameters);
}

int Process::get_rank() const
{
  return rank_;
}

int Process::get_epoch() const
{
  return i_epoch_;
}
Type Process::get_type() const
{
  return type_;
}
int Process::get_time_to_save() const
{
  return parameters_.time_save;
}

void Process::set_type(Type type)
{
  type_ = type;
}
void Process::upgrade_to_master(std::vector<int> masters)
{
  type_ = Type::Master;
  masters_ = masters;
}

void Process::save_nn(const std::string& filename, int n_epoch) const
{
  nn_.save(filename, n_epoch);
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


/**
 * Sends the rank to the right node
 * If right is dead, the ring is reconfigurated to exclude the dead node.
 * If I receive a Tag::Election, I answer with Tag::ElectionConfirmation.
 * If I receive a Tag::Election from someone that is not my left, I put it's rank as my left.
 * If I receive a Tag::Election containing my rank, I am the president
 * and I answer it with Tag::EndElection.
 * When a node is answered by the right node with Tag::EndElection, there are 2 cases:
 *  -If my left node isn't the president, I wait a Tag::Election from him and answer with Tag::EndElection
 *  -If my left node is the president, I send him Tag::EndElection 
 *    (meaning that everyone in the ring knows that the election is over)
 * When president receives Tag::EndElection from right, that means that the election is over
 * and everybody knows that it's the President.
 */
void Process::elect_president()
{
  //std::cout << rank_ << " Begin President" << std::endl;
  bool has_ended = false;
  bool should_end = false;
  int i = 0;
  while (!has_ended)
  {
    int rec, rec2;
    MPI_Request reqr, reqr2;
    int flag = 0;
    int flag2 = 0;
    MPI_Status status, status2;
    MPI_Irecv(&rec, 1, MPI_INT, MPI_ANY_SOURCE, Tag::Election, MPI_COMM_WORLD, &reqr);
    MPI_Irecv(&rec2, 1, MPI_INT, MPI_ANY_SOURCE, Tag::Endelection, MPI_COMM_WORLD, &reqr2);
    MPI_Send(&president_id_, 1, MPI_INT, right_id_, Tag::Election, MPI_COMM_WORLD);
    MPI_Test(&reqr, &flag, &status);
    MPI_Test(&reqr2, &flag2, &status2);
    while (!flag && !flag2)
    {
      MPI_Test(&reqr, &flag, &status);
      MPI_Test(&reqr2, &flag2, &status2);
    }
    if (flag)
    {
      MPI_Cancel(&reqr2);
    }
    else if (flag2)
    {
      MPI_Cancel(&reqr);
      has_ended = true;
      break;
    }
    if (flag && rec > president_id_)
      president_id_ = rec;
    else if (flag && rec == rank_)
    {
      president_id_ = rank_;
      type_ = Type::President;
      has_ended = true;
      std::cout << rank_ << " is the president" << std::endl;
    }
    int rec3;
    MPI_Request reqr3;
    MPI_Status status3;
    MPI_Irecv(&rec3, 1, MPI_INT, MPI_ANY_SOURCE, Tag::ElectionConfirmation, MPI_COMM_WORLD, &reqr3);
    MPI_Send(&president_id_, 1, MPI_INT, left_id_, Tag::ElectionConfirmation, MPI_COMM_WORLD);
    MPI_Wait(&reqr3, &status3);
  }
  if (type_ == Type::President)
  {
    int rec, rec2;
    MPI_Request reqr, reqr2;
    int flag = 0;
    MPI_Status status, status2;
    MPI_Irecv(&rec, 1, MPI_INT, MPI_ANY_SOURCE, Tag::Election, MPI_COMM_WORLD, &reqr);
    MPI_Send(&president_id_, 1, MPI_INT, right_id_, Tag::Endelection, MPI_COMM_WORLD);
    MPI_Wait(&reqr, &status);
    MPI_Send(&president_id_, 1, MPI_INT, left_id_, Tag::ElectionConfirmation, MPI_COMM_WORLD);
    while(1)
    {
      MPI_Recv(&rec2, 1, MPI_INT, left_id_, MPI_ANY_SOURCE, MPI_COMM_WORLD, &status);
      if (status.MPI_TAG == Tag::Endelection)
        break;
      else
        MPI_Send(&president_id_, 1, MPI_INT, left_id_, Tag::ElectionConfirmation, MPI_COMM_WORLD);
    }
  }
  else
  {
    int rec, rec2;
    MPI_Request reqr, reqr2;
    int flag = 0;
    int flag2 = 0;
    MPI_Status status, status2;
    MPI_Recv(&rec, 1, MPI_INT, right_id_, Tag::ElectionConfirmation, MPI_COMM_WORLD, &status);
    MPI_Send(&president_id_, 1, MPI_INT, right_id_, Tag::Endelection, MPI_COMM_WORLD);
  }
}

void Process::elect_masters()
{
  // TODO
  // Fill masters_ and workers_ with their ranks and send type to masters
  int n_masters = world_size_ * parameters_.ratio;

  int workers_start = 0;
  for (int i = 0; i < world_size_ && masters_.size() < n_masters; i++)
  {
    if (i == rank_)
      continue;
    masters_.push_back(i);
    workers_start = i;
  }

  for (auto m: masters_)
  {
    MPI_Send(masters_.data(), masters_.size(), MPI_INT, m, Tag::UpgradeToMaster, MPI_COMM_WORLD);
  }
  for (int i = workers_start + 1; i < world_size_; i++)
  {
    if (i == rank_)
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
    else if (param == "time_save")
      parameters_.time_save = std::stod(value);
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
  for (auto w: workers_)
    send_weights(w);
}

void Process::send_weights_to_master()
{
  for (auto w: masters_)
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
