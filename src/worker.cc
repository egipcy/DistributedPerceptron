#include "worker.hh"

/**
 * add data in the constructor
*/
Worker::Worker(int id, std::vector<int> ids_masters, std::vector<int> ids_workers)
  : id_(id)
  , ids_masters_(ids_masters)
  , ids_workers_(ids_workers)
  , awake_(true)
  , president_id_(-1)
{ }

void Worker::start()
{
  // Wait for datas
  datas_ = get_datas();
}

/**
 * Should be an int to handle 3 cases :
 *  is alive
 *  is dead
 *  should become a master
*/
bool Worker::run()
{
  if (!is_awake())
    return true;
  
  // Wait for weights
  weights_ = get_weights();

  gradients_ = compute_gradients();
  broadcast_gradients();

  return true;
}

bool Worker::is_awake() const
{
  return awake_;
}

void Worker::awake()
{
  awake_ = true;
}

void Worker::sleep()
{
  awake_ = false;
}

int Worker::get_president() const
{
  // TODO
  return -1;
}

/**
 * get range
*/
std::pair<Matrix, std::vector<double>> Worker::get_datas() const
{
  // TODO
  return std::pair<Matrix, std::vector<double>>();
}

std::vector<Matrix> Worker::get_weights() const
{
  // TODO
  return std::vector<Matrix>();
}

std::vector<Matrix> Worker::compute_gradients() const
{
  // TODO
  return std::vector<Matrix>();
}

void broadcast_gradients() const
{
  // TODO
}