#include "nn.hh"

#include <cassert>
#include <iostream>

NN::NN(const std::vector<Matrix>& weights, const std::vector<Matrix>& biases)
  : weights_(weights)
  , biases_(biases)
{ }

NN::NN(const std::vector<int>& nb_neurons)
{
  assert(nb_neurons.size() >= 2);
  for (size_t i = 0; i < nb_neurons.size(); i++)
    assert(nb_neurons[i] > 0);

  // Initializes weights
  for (size_t i = 0; i < nb_neurons.size() - 1; i++)
  {
    weights_.emplace_back(nb_neurons[i], nb_neurons[i + 1]);
    weights_[i].randomize();
  }

  for (size_t i = 0; i < weights_.size(); i++)
  {
    biases_.emplace_back(1, nb_neurons[i + 1]);
    biases_[i].randomize();
  }
}

std::pair<Matrix, std::vector<Matrix>> NN::forward(const Matrix& inputs)
{
  assert(inputs.columns() == weights_[0].columns());

  std::vector<Matrix> weighted_sums;
  Matrix activated_sum(inputs);

  for (size_t i_layer = 0; i_layer < weights_.size(); i_layer++)
  {
    weighted_sums.emplace_back(activated_sum.dot(weights_[i_layer]) + biases_[i_layer][0][0]);
    activated_sum = weighted_sums[i_layer].map(NN::activation_function);
  }

  return std::pair(activated_sum, weighted_sums);
}

std::pair<std::vector<Matrix>, std::vector<Matrix>> NN::backpropagation(
  const Matrix& inputs, const Matrix& labels)
{
  assert(inputs.rows() == labels.rows());
  assert(inputs.columns() == weights_[0].rows());
  assert(labels.columns() == weights_[weights_.size() - 1].columns());

  auto pair = forward(inputs);
  Matrix results(pair.first);
  auto weighted_sums = pair.second;

  std::vector<Matrix> deltas(weights_.size());

  // Calculates delta in output layer
  deltas[weights_.size() - 1] = (results - labels) * weighted_sums[weighted_sums.size() - 1].map(NN::derivate_function);

  // Calculates deltas in hidden layers
  for (size_t i = 1; i < weights_.size(); i++)
  {
    size_t i_layer = weights_.size() - i - 1;
    deltas[i_layer] = deltas[i_layer + 1].dot(weights_[i_layer + 1].transpose()) * weighted_sums[i_layer].map(NN::derivate_function);
  }

  // Calculates and stores gradients
  std::vector<Matrix> gradients_w(weights_.size());
  gradients_w[0] = inputs.transpose().dot(deltas[0]);

  for (size_t i_layer = 1; i_layer < weights_.size(); i_layer++)
    gradients_w[i_layer] = weighted_sums[i_layer - 1].map(NN::activation_function).transpose().dot(deltas[i_layer]);

  return std::pair(gradients_w, deltas);
}

void NN::update_simple(const std::vector<Matrix>& gradients_w,
  const std::vector<Matrix>& gradients_b, double learning_rate)
{
  assert(gradients_w.size() == gradients_b.size());
  assert(gradients_w.size() == weights_.size());

  for (size_t i = 0; i < weights_.size(); i++)
  {
    weights_[i] -= learning_rate * gradients_w[i];
    biases_[i] -= learning_rate * Matrix(1, gradients_b[i].rows(), 1.0).dot(gradients_b[i]);
  }
}

void NN::update_delayed1(const std::vector<Matrix>& gradients_w,
  const std::vector<Matrix>& gradients_b, double learning_rate,
  const std::vector<Matrix>& old_weights, const std::vector<Matrix>& old_biases,
  double lambda)
{
  // TODO
}

void NN::update_delayed2(const std::vector<Matrix>& gradients_w,
  const std::vector<Matrix>& gradients_b, double learning_rate,
  const std::vector<Matrix>& old_weights, const std::vector<Matrix>& old_biases,
  double lambda)
{
  // TODO
}

void NN::print() const
{
  std::cout << "Weights:" << std::endl;
  for (size_t i = 0; i < weights_.size(); i++)
    weights_[i].print();

  std::cout << "Biases:" << std::endl;
  for (size_t i = 0; i < biases_.size(); i++)
    biases_[i].print();
}