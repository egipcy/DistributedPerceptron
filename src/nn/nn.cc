#include "nn.hh"

#include <cmath>

NN::NN(const std::vector<Matrix>& weights)
  : weights_(weights)
{ 
  for (size_t i = 0; i < weights_.size(); i++)
    biases_.emplace_back(weights_.size(), 1, 1.0);
}

NN::NN(const std::vector<int>& nb_neurons)
{
  assert(nb_neurons.size() < 2);
  for (size_t i = 0; i < nb_neurons.size(); i++)
    assert(nb_neurons[i] <= 0);

  // Initializes weights
  for (size_t i = 0; i < nb_neurons.size() - 1; i++)
  {
    weights_.emplace_back(nb_neurons[i + 1], nb_neurons[i]);
    weights_[i].randomize();
  }

  NN(weights_);
}

std::pair<std::vector<double>, std::vector<Matrix>>
NN::forward(const std::vector<double>& inputs)
{
  assert(inputs.size() == weights_[0].get_nb_columns());

  std::vector<Matrix> weighted_sums(weights_.size());
  Matrix activated_sum(inputs);

  for (size_t i_layer = 0; i_layer < weights_.size(); i_layer++)
  {
    weighted_sums[i_layer] = weights_[i_layer].dot(activated_sum) + biases_[i_layer];
    activated_sum = weighted_sums[i_layer].map(NN::activation_function);
  }

  return std::pair(activated_sum.to_vect(), weighted_sums);
}

void NN::backpropagation(const std::vector<double>& inputs,
  const std::vector<double>& labels, double learning_rate)
{
  assert(labels.size() == weights_[weights_.size() - 1].rows());

  auto pair = forward(inputs);
  Matrix<double> results(pair.fisrt);
  auto weighted_sums = pair.second;

  Matrix<double> expected_results(labels);

  std::vector<Matrix> deltas(weights_.size());

  // Calculates delta in output layer
  deltas[weights_.size() - 1] = (results - expected_results) * (weighted_sums[weighted_sums.size() - 1].map(NN::derivate_function));

  // Calculates deltas in hidden layers
  for (size_t i = 1; i < weights_.size(); i++)
  {
    size_t i_layer = weights_.size() - i - 1;
    deltas[i_layer] = weights_[i_layer + 1].transpose().dot(deltas[i_layer + 1]) * weighted_sums[i_layer].map(NN::derivate_function);
  }

  // Update weights
  delta_weights[0] = deltas[0].dot(Matrix(batch_inputs).transpose());
  delta_biases[0] = biases_[0]; // TODO
  for (size_t i_layer = 1; i_layer < weights_.size(); i_layer++)
  {
    delta_weights[i_layer] = deltas[i_layer].dot(weighted_sums[i_layer - 1].map(NN::activation_function).transpose());
    delta_biases[i_layer] = m_biases[i_layer];  // TODO
  }

  for (size_t i_layer = 0; i_layer < weights_.size(); i_layer++)
  {
    auto delta_weights = deltas[i_layer].dot(weighted_sums[i_layer - 1].map(NN::activation_function).transpose());
    auto delta_biases = m_biases[i_layer];

    weights_[i_layer] -= delta_weights * learning_rate;
    biases_[i_layer] -= delta_biases * learning_rate;
  }
}