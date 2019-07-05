#include "master.hh"


Master::Master(int id, std::vector<int> ids_masters, std::vector<int> ids_workers, int nb_epochs,
  std::pair<Matrix, std::vector<double>> datas)
  : id_(id)
  , ids_masters_(ids_masters)
  , ids_workers_(ids_workers)
  , nb_epochs_(nb_epochs)
  , datas_(datas)
  , awake_(true)
  , president_id_(-1)
{ }

void Master::start()
{
  president_id_ = elect_president();
  broadcast_president();

  if (president_id_ != id_)
    return;

  splitted_datas_ = split_datas();
  broadcast_datas();

  weights_ = build_weights();
}

bool Master::run()
{
  if (!is_awake() || president_id_ != id_)
    /**
     * wait the weights
     * put a timer to start a new president election if any data received
    */
    return true;

  broadcast_weights();

  // Wait for gradients
  get_gradients();
  update_weights();

  return --nb_epochs_ == 0;
}

bool Master::is_awake() const
{
  return awake_;
}

void Master::awake()
{
  awake_ = true;
}

void Master::sleep()
{
  awake_ = false;
}

int Master::elect_president() const
{
  /**
   * elects a president between masters_ids
  */
  return this->id_;
}

void Master::broadcast_president() const
{
  //sends to workers (masters already know him)
  // TODO
}

/**
 * creates a range
*/
std::vector<std::pair<Matrix, std::vector<double>>> Master::split_datas() const
{
  // TODO
  // The length of the returned vector is the amount of workers
  // Clear datas when finished (use std::move)
  return std::vector<std::pair<Matrix, std::vector<double>>>();
}

/**
 * sends the range
*/
void Master::broadcast_splitted_datas() const
{
  assert(splitted_datas_.count() == ids_workers_.count());

  // TODO
}

std::vector<Matrix> Master::build_weights() const
{
  // TODO
  return std::vector<Matrix>();
}

void Master::broadcast_weights() const
{
  // TODO
}

void get_gradients()
{
  // TODO
}

void update_weights()
{
  // TODO
}
