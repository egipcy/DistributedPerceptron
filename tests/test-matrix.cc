#include <iostream>
#include <cmath>

#include "../src/matrix/matrix.hh"

void print(const Matrix& matrix)
{
  std::cout << matrix.rows() << ";" << matrix.columns() << std::endl;
  for (int i = 0; i < matrix.rows(); i++)
  {
    for (int j = 0; j < matrix.columns(); j++)
      std::cout << matrix[i][j] << "\t";
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int main()
{
  Matrix m(2, 3, 1);
  print(m);
  m[1][2] = 10;
  m[0][1] = 4;
  print(m);

  Matrix m2(3, 3);
  for (int i = 0; i < m2.rows(); i++)
    for (int j = 0; j < m2.columns(); j++)
      m2[i][j] = i + j * 2;
  print(m2);

  print(m.dot(m2));
  print(((m+1)*2).map(std::sqrt));

  return 0;
}