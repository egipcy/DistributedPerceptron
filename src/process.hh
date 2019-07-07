#pragma once

#include <fstream>
#include <vector>
#include "nn/nn.hh"
#include "matrix/matrix.hh"

struct Parameters
{
  int nb_hidden_layers = 2,
  int nb_hidden_neurons = 10
};

enum class Type
{
  President,
  Master,
  Worker
};

class Process
{
public:
  Process(int rank, int world_size, std::string filename_data, std::string filename_parameters, double ratio, int nb_epochs);

  int get_rank() const;

  Type get_type() const;
  void set_type(Type type);

  NN& get_nn();

  bool is_alive() const;
  void set_alive(bool alive);

  bool has_ended() const;

  void elect_president();
  void elect_masters();
  void send_ranges();

private:
  int rank_;
  int w_size_;
  int president_id_;
  std::vector<int> masters_;

  Type type_;
  double ratio_;

  NN nn_;
  int nb_epochs_;

  std::pair<Matrix, Matrix> datas_;
  std::pair<int, int> range_;

  bool alive_;
  bool has_ended_;

  Parameters parameters_;
};