#include "worker.hh"

Worker::Worker(int id, std::vector<int> ids_masters, std::vector<int> ids_workers)
  : id_(id)
  , ids_masters_(ids_masters)
  , ids_workers_(ids_workers)
  , awake_(true)
  , president_id_(-1)
{ }

void Worker::start()
{
  // Wait for president election
  president_id_ = get_president();

  // Wait for datas
  datas_ = get_datas();
}

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