#pragma once

#include "../matrix/matrix.hh"

#include <cmath>

class NN
{
public:
  NN() = delete;
  NN(const std::vector<Matrix>& weights, const std::vector<Matrix>& biases);
  NN(const std::vector<int>& nb_neurons);

  /*std::pair<std::vector<double>, std::vector<Matrix>>
  forward(const std::vector<double>& inputs);

  std::pair<std::vector<Matrix>, std::vector<Matrix>> backpropagation(
    const std::vector<double>& inputs, const std::vector<double>& labels);*/

  std::pair<Matrix, std::vector<Matrix>> forward(const Matrix& inputs);

  std::pair<std::vector<Matrix>, std::vector<Matrix>> backpropagation(
    const Matrix& inputs, const Matrix& labels);

  void update_simple(const std::vector<Matrix>& gradients_w,
    const std::vector<Matrix>& gradients_b, double learning_rate);

  void update_delayed1(const std::vector<Matrix>& gradients_w,
    const std::vector<Matrix>& gradients_b, double learning_rate,
    const std::vector<Matrix>& old_weights, const std::vector<Matrix>& old_biases,
    double lambda);

  void update_delayed2(const std::vector<Matrix>& gradients_w,
    const std::vector<Matrix>& gradients_b, double learning_rate,
    const std::vector<Matrix>& old_weights, const std::vector<Matrix>& old_biases,
    double lambda);

  void print() const;

private:
  std::vector<Matrix> weights_;
  std::vector<Matrix> biases_;

  static double activation_function(double x)
  {
    return 1.0 / (1.0 + std::exp(-x));
  }

  static double derivate_function(double x)
  {
    return activation_function(x) * (1.0 - activation_function(x));
  }
};