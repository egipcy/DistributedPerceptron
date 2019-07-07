#include <cstdlib>
#include <cassert>
#include <iostream>

#include "matrix.hh"

Matrix::Matrix()
{ }

Matrix::Matrix(int rows, int columns, double init_value)
  : rows_(rows)
  , columns_(columns)
{
  for (int i = 0; i < rows_; i++)
    mat_.emplace_back(columns_, init_value);
}

Matrix::Matrix(std::vector<double> v)
  : rows_(v.size())
  , columns_(1)
{
  for (int i = 0; i < rows_; i++)
    mat_.emplace_back(1, v[i]);
}

Matrix(std::vector<std::vector<double>> mat)
  : rows_(mat.size())
  , columns_(mat[0].size())
  , mat_(mat)
{
  for (size_t i = 1; i < mat.size(); i++)
    assert(mat[i].size() == mat[0].size());
}

void Matrix::randomize()
{
  for (auto& line: mat_)
    for (auto& e: line)
      e = std::rand() / (double)RAND_MAX;
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

Matrix Matrix::operator-() const
{
  Matrix ret(*this);

  for (auto& line: ret.mat_)
    for (auto& e: line)
      e = -e;

  return ret;
}

Matrix& Matrix::operator-=(double n)
{
  return operator+=(-n);
}

Matrix& Matrix::operator-=(const Matrix& other)
{
  *this += -other;
  return *this;
}

Matrix Matrix::operator-(double n) const
{
  return operator+(-n);
}

Matrix Matrix::operator-(const Matrix& other) const
{
  return *this + (-other);
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

Matrix Matrix::transpose() const
{
  Matrix ret(columns(), rows());

  for (int i = 0; i < columns(); i++)
    for (int j = 0; j < rows(); j++)
      ret[i][j] = (*this)[j][i];

  return ret;
}

Matrix Matrix::dot(const Matrix& other) const
{
  assert(columns() == other.rows());

  Matrix ret(rows(), other.columns());

  for (int i = 0; i < rows(); i++)
    for (int j = 0; j < other.columns(); j++)
    {
      double s = 0.0;
      for (int k = 0; k < columns(); k++)
        s += mat_[i][k] * other[k][j];
      ret[i][j] = s;
    }

  return ret;
}

Matrix Matrix::map(double (*func) (double)) const
{
  Matrix ret(*this);

  for (auto& line: ret.mat_)
    for (auto& e: line)
      e = func(e);

  return ret;
}

std::vector<double> Matrix::to_vect() const
{
  assert(columns_ == 1);

  std::vector<double> ret(rows_);
  for (int i = 0; i < rows_; i++)
    ret[i] = mat_[i][0];

  return ret;
}

void Matrix::print() const
{
  std::cout << rows() << ";" << columns() << std::endl;
  for (int i = 0; i < rows(); i++)
  {
    for (int j = 0; j < columns(); j++)
      std::cout << mat_[i][j] << "\t";
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

Matrix operator+(double n, const Matrix& other)
{
  return other + n;
}

Matrix operator-(double n, const Matrix& other)
{
  return other - n;
}

Matrix operator*(double n, const Matrix& other)
{
  return other * n;
}