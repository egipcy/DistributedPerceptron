#pragma once

#include "matrix/matrix.hh"

class Master
{
public:
  Master(int id, std::vector<int> ids_masters, std::vector<int> ids_workers, int nb_epochs,
    std::pair<Matrix, std::vector<double>> datas);

  void start();
  bool run();

  bool is_awake() const;
  void awake();
  void sleep();

private:
  int id_;
  std::vector<int> ids_masters_;
  std::vector<int> ids_workers_;
  int nb_epochs_;
  std::pair<Matrix, std::vector<double>> datas_;

  bool awake_;

  int president_id_;
  int elect_president() const;
  void broadcast_president() const;

  std::vector<std::pair<Matrix, std::vector<double>>> splitted_datas_;
  std::vector<std::pair<Matrix, std::vector<double>>> split_datas() const;
  void broadcast_splitted_datas() const;

  std::vector<Matrix> weights_;
  std::vector<Matrix> build_weights() const;
  void broadcast_weights() const;

  std::vector<Matrix> gradients_;
  void get_gradients();
  void update_weights();
};