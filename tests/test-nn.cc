#include "../src/nn/nn.hh"

#include <iostream>
#include <cassert>
#include <ctime>
#include <vector>

void print_results(const std::vector<double>& results, const std::vector<double>& labels)
{
  assert(results.size() == labels.size());

  std::cout << "(";
  for (size_t i = 0; i < results.size() - 1; i++)
    std::cout << results[i] << ",";
  std::cout << results[results.size() - 1] << ")";

  std::cout << "->(";
  for (size_t i = 0; i < labels.size() - 1; i++)
    std::cout << labels[i] << ",";
  std::cout << labels[labels.size() - 1] << ")";

  double loss = 0.0;
  for (size_t i = 0; i < results.size(); i++)
    loss += (results[i] - labels[i]) * (results[i] - labels[i]);
  std::cout << " l=" << loss << std::endl;
}

int main()
{
  std::srand(std::time(nullptr));

  NN nn({2, 3, 1});
  nn.print();

  Matrix inputs(4, 2);
  inputs[0] = {0, 0};
  inputs[1] = {0, 1};
  inputs[2] = {1, 0};
  inputs[3] = {1, 1};
  Matrix labels(4, 1);
  labels[0] = {0};
  labels[1] = {1};
  labels[2] = {1};
  labels[3] = {0};


  auto p1 = nn.forward(inputs);
  for (int i = 0; i < 4; i++)
    print_results(p1.first[i], labels[i]);
  std::cout << std::endl;

  std::cout << "Backpropagation..." << std::endl;
  for (int k = 0; k < 100000; k++)
  {
    nn.forward(inputs);
    auto p = nn.backpropagation(inputs, labels);
    nn.update_simple(p.first, p.second, 0.1);
  }

  std::cout << std::endl << "Results" << std::endl;
  nn.print();

  p1 = nn.forward(inputs);
  for (int i = 0; i < 4; i++)
    print_results(p1.first[i], labels[i]);
  std::cout << std::endl;

  return 0;
}