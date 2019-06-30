#pragma once

#include "../matrix/matrix.hh"

class NN
{
public:
  NN() = delete;
  NN(const std::vector<Matrix>& weights);
  NN(const std::vector<int>& nb_neurons);

  std::pair<std::vector<double>, std::vector<Matrix>>
  forward(const std::vector<double>& inputs);

  void backpropagation(const std::vector<double>& inputs,
    const std::vector<double>& labels, double learning_rate);

private:
  std::vector<Matrix> weights_;
  std::vector<Matrix> biases_;

  static double activation_function(double x)
  {
    return 1.0;
  }

  static double derivate_function(double x)
  {
    return 1.0;
  }
};