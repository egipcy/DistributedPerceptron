#pragma once
#include <iostream>
#include "matrix/matrix.hh"

class Process
{
public:
  Process(int id, int world_size, const std::pair<Matrix, std::vector<double>>* data);

  void start();
  void elect_president(int world_rank, int world_size);
  void set_president(int president_id);
private:
  const int id_;
  const int world_size_;
  const int nb_epochs_;
  int president_id_;
  Matrix* weights_;
  const std::pair<Matrix, std::vector<double>>* data_;
};