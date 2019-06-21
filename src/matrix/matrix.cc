#pragma once

#include <ctime>

Matrix::Matrix(int rows, int columns, double init_value)
{
  for (int i = 0; i < rows; i++)
    mat_.emplace_back(columns, init_value);

  std::srand(std::time(nullptr));
}

void Matrix::randomize()
{
  for (auto& line: mat_)
    for (auto& e: line)
      e = (std::rand() / (double)RAND_MAX - 0.5) * 2;
}

int Matrix::rows() const
{
  return rows_;
}

int Matrix::columns() const
{
  return columns_;
}

std::vector<double>& Matrix::operator[](int index)
{
  return mat_[index];
}

std::vector<double> Matrix::operator[](int index) const
{
  return mat_[index];
}

Matrix& Matrix::operator+=(double n)
{
  for (auto& line: mat_)
    for (auto& e: line)
      e += n;

  return *this;
}

Matrix& Matrix::operator+=(const Matrix& other)
{
  assert(other.rows() == rows() && other.columns() == columns());

  for (int i = 0; i < rows(); i++)
    for (int j = 0; j < columns(); j++)
      mat_[i][j] += other[i][j];

  return *this;
}

Matrix Matrix::operator+(double n) const
{
  Matrix ret(*this);

  ret += n;

  return ret;
}

Matrix Matrix::operator+(const Matrix& other) const
{
  Matrix ret(*this);

  ret += other;

  return ret;
}

Matrix& Matrix::operator*=(double n)
{
  for (auto& line: mat_)
    for (auto& e: line)
      e *= n;

  return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) // Hadamard product
{
  assert(other.rows() == rows() && other.columns() == columns());

  for (int i = 0; i < rows(); i++)
    for (int j = 0; j < columns(); j++)
      mat_[i][j] *= other[i][j];

  return *this;
}

Matrix Matrix::operator*(double n) const
{
  Matrix ret(*this);

  ret *= n;

  return ret;
}

Matrix Matrix::operator*(const Matrix& other) const // Hadamard product
{
  Matrix ret(*this);

  ret *= other;

  return ret;
}

Matrix Matrix::dot(const Matrix& other) const
{
  assert(columns() == other.rows());

  Matrix ret(rows(), other.columns());

  for (int i = 0; i < rows(); i++)
    for (int j = 0; j < other.columns(); j++)
    {
      int s = 0;
      for (int k = 0; k < columns())
        s += mat_[i][k] * other[k][j];
      ret[i][j] = s;
    }

  return ret;
}

Matrix Matrix::map(const double (*func) (double)&) const
{
  Matrix ret(*this);

  for (auto& line: ret.mat_)
    for (auto& e: line)
      e = func(e);

  return ret;
}