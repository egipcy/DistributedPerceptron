#pragma once

#include "matrix.hh"

class Worker
{
public:
  Worker(int id, std::vector<int> ids_masters, std::vector<int> ids_workers);

  void start();
  bool run();

  bool is_awake() const;
  void awake();
  void sleep();

private:
  int id_;
  std::vector<int> ids_masters_;
  std::vector<int> ids_workers_;

  bool awake_;

  int president_id_;
  int get_president() const;

  std::pair<Matrix, std::vector<double>> datas_;
  std::pair<Matrix, std::vector<double>> get_datas() const;

  std::vector<Matrix> weights_;
  std::vector<Matrix> get_weights() const;

  std::vector<Matrix> gradients_;
  std::vector<Matrix> compute_gradients() const;
  void broadcast_gradients() const;
};