#pragma once

#include <fstream>
#include <vector>
#include "nn/nn.hh"
#include "matrix/matrix.hh"

struct Parameters
{
  int nb_hidden_layers = 2;
  int nb_hidden_neurons = 10;
  double learning_rate = 0.01;
};

enum class Type
{
  President,
  Master,
  Worker
};

enum Tag
{
  WeightsMatrix = 1,
  BiasesMatrix
};

class Process
{
public:
  Process(int rank, int world_size, const std::string& filename_data,
    const std::string& filename_parameters, double ratio, int nb_epochs);

  int get_rank() const;

  Type get_type() const;
  void set_type(Type type);

  NN& get_nn();

  bool is_alive() const;
  void set_alive(bool alive);

  bool has_ended() const;

  void elect_president();
  void elect_masters();

  /* Communication */
  void send_weights(int dest);
  void set_weights_biases(const std::vector<double>& weights, const std::vector<double>& biases);

private:
  int rank_;
  int world_size_;
  int president_id_;
  std::vector<int> masters_;
  std::vector<int> workers_;

  Type type_;
  double ratio_;

  NN nn_;
  int nb_epochs_;

  std::pair<Matrix, Matrix> datas_;
  void init_datas(const std::string& filename_data);

  bool alive_;
  bool has_ended_;

  Parameters parameters_;
  void init_parameters(const std::string& filename_parameters);
};