#include <iostream>
#include <cassert>
#include <ctime>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream>

#include "nn/nn.hh"

double print_results(const std::vector<double>& results, const std::vector<double>& labels)
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

  return loss;
}

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cerr << "Wrong number of arguments" << std::endl;
    std::cerr << "Arguments : neural network's path, datas' path" << std::endl;
    return 1;
  }

  std::srand(std::time(nullptr));

  NN nn;
  nn.load(argv[1]);

  std::ifstream file_data(argv[2], std::ios::in);
  if(!file_data.is_open())
  {
    std::cerr << "File not found: " << argv[2] << std::endl;
    exit(1);
  }

  // Read datas
  std::string line;
  std::vector<std::vector<double>> X;
  std::vector<double> Y;
  while (std::getline(file_data, line))
  {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(","));

    std::vector<double> x_double(result.size() - 1);
    for (size_t i = 0; i < x_double.size(); i++)
      x_double[i] = std::stod(result[i]);
    double y_double = std::stod(result[result.size() - 1]);

    X.push_back(x_double);
    Y.push_back(y_double);
  }

  file_data.close();


  // Print results
  auto p = nn.forward(X);
  double loss = 0;
  for (int i = 0; i < 4; i++)
    loss += print_results(p.first[i], Matrix(Y)[i]);
  std::cout << "Total Loss = " << loss << std::endl;

  return 0;
}