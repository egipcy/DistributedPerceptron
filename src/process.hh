#pragma once
#include <iostream>
#include "matrix/matrix.hh"

class Process
{
public:
  Process(int id, int world_size);

  void start();
  void elect_president(int world_rank, int world_size);
  void set_president(int president_id);
private:
  const int id_;
  int world_size_;
  int nb_epochs_;
  int president_id_;

};