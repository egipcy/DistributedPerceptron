#pragma once
#include "matrix/matrix.hh"

class Process
{
public:
  Process(int id, int world_size);

  void start();
  void elect_president();

private:
  const int id_;
  int world_size_;
  int nb_epochs_;
};