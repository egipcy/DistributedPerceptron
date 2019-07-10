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
  int nb_epochs = 1000;
  double ratio = 0.3;
  int time_save = 100;
  int formula = 2;
  double lambda = 0.04;
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
  BiasesMatrix,
  Finished,
  Election,
  ElectionConfirmation, //Means I received your Id
  Endelection,
  UpgradeToMaster,
  StoreWorkers
};

class Process
{
public:
  Process(int rank, int world_size, const std::string& filename_data,
    const std::string& filename_parameters);

  int get_rank() const;
  Type get_type() const;
  int get_time_to_save() const;
  int get_president() const;
  void set_president(const int president_id);
  int get_epoch() const; 
  bool get_need_load() const;

  // void set_i_epoch(int currently_epochs);
  void set_need_load();
  void set_type(Type type);
  void upgrade_to_master(const std::vector<int>& masters);
  void save_workers(const std::vector<int>& workers);
  void master_to_president();

  void save_nn(const std::string& filename, int n_epoch);
  void load_nn(const std::string& filename);
  bool is_alive() const;
  void set_alive(bool alive);

  bool has_ended() const;
  void end();
  void end_all() const;

  void send_token(const int tag, int& electionEnded, int& last_sent_rank);
  void elect_president();
  void elect_masters();
  void init_nn();

  const Parameters& get_parameters() const;


  /* Communication */
  void send_weights(int dest);
  void send_weights_all();
  void send_weights_to_master();

  void set_weights_biases(const std::vector<double>& weights, const std::vector<double>& biases);
  std::pair<std::vector<Matrix>, std::vector<Matrix>> get_gradients();
  void update_nn(const std::vector<double>& gradients_w, const std::vector<double>& gradients_b);
  void update_nn_delayed1(const std::vector<double>& gradients_w,
  const std::vector<double>& gradients_b, int source, double lambda);
  void update_nn_delayed2(const std::vector<double>& gradients_w,
  const std::vector<double>& gradients_b, int source, double lambda);

private:
  int rank_;
  int world_size_;
  int left_id_;
  int right_id_;
  int president_id_;
  std::vector<int> masters_;
  std::vector<int> workers_;

  Type type_;

  NN nn_;
  std::vector<NN> old_nns_;
  int i_epoch_;

  std::pair<Matrix, Matrix> datas_;
  void init_datas(const std::string& filename_data);

  bool alive_;
  bool has_ended_;

  Parameters parameters_;
  void init_parameters(const std::string& filename_parameters);
  bool need_load_file;
};
