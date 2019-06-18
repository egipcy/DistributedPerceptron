#pragma once

class Master
{
public:
  Master(int id, std::vector<int> ids_masters, std::vector<int> ids_workers, int nb_epochs,
    std::pair<std::vector<std::vector<double>>, std::vector<double>> datas);

  void start();

  bool has_ended() const;

  bool is_awake() const;
  void awake();
  void sleep();

private:
  int id_;
  std::vector<int> ids_masters_;
  std::vector<int> ids_workers_;
  int nb_epochs_;
  std::pair<std::vector<std::vector<double>>, std::vector<double>> datas_;

  bool ended_;
  bool awake_;

  int elect_president();
  void broadcast_president(int id_president);
};