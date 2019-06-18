#pragma once

class Worker
{
public:
  Worker(int id, std::vector<int> ids_masters, std::vector<int> ids_workers);

  void start();

  bool has_ended() const;

  bool is_awake() const;
  void awake();
  void sleep();

private:
  int id_;
  std::vector<int> ids_masters_;
  std::vector<int> ids_workers_;
  std::pair<std::vector<std::vector<double>>, std::vector<double>> datas_;

  bool ended_;
  bool awake_;

  void update_president();
};