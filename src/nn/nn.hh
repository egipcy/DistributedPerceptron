#pragma once

#include "../matrix/matrix.hh"

#include <cmath>
#include <string>

class NN
{
public:
  NN();
  NN(const std::vector<Matrix>& weights, const std::vector<Matrix>& biases);
  NN(const std::vector<int>& nb_neurons);

  const std::vector<Matrix>& get_weights() const;
  const std::vector<Matrix>& get_biases() const;

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

  void save(const std::string& filename, int i_epochs) const;
  int load(const std::string& filename);

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
