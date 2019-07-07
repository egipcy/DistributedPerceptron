#pragma once

#include <fstream>
#include <vector>

#include "matrix/matrix.hh"

enum class Type
{
  President,
  Master,
  Worker
};

class Process
{
public:
  Process(int rank, int world_size, std::string filename, double ratio, int nb_epochs);

  int get_rank() const;

  Type get_type() const;
  void set_type(Type type);

  NN& get_nn();

  void elect_president();

private:
  int rank_;
  int world_size_;
  Type type_;
  NN nn_;
  double ratio_;
  int nb_epochs_;
  std::pair<Matrix, Matrix> datas_;
  std::pair<int, int> range_;
};