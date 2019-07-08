#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iterator>
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

Matrix::Matrix(std::vector<std::vector<double>> mat)
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
std::vector<std::vector<double>> Matrix::mat()
{
  return mat_;
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
  std::vector<double> flatten(std::vector<std::vector<double>> vect)
{
  std::vector<double> flatten(std::begin(vect[0]),std::end(vect[0]));
  for (int i = 1; i < vect.size();i++)
    flatten.insert(std::end(flatten),std::begin(vect[i]), std::end(vect[i]));

  return flatten;
}

std::vector<double> serialize(std::vector<Matrix> vect)
{
  std::vector<double> res;
  for(int i =0; i< vect.size(); i++)
  {
    auto tmp = flatten(vect[i].mat());
    res.insert(std::end(res), std::begin(tmp), std::end(tmp));
  } 
  for(int i = 0; i < vect.size();i++)
    res.push_back(vect[i].columns());    
 
  for(int i = 0; i < vect.size();i++)
    res.push_back(vect[i].rows());

  res.push_back(vect.size());
  return res;
}

 std::vector<Matrix> deserialize(std::vector<double> vect)
{
  std::vector<Matrix> res;
  int old_pos = 0;
  int nb_matrix = vect[vect.size()-1];

  for(int nb = 0;nb< nb_matrix; nb++)
   {
     std::vector<std::vector<double>> mat;
     int col = vect[vect.size()-1 -(2*nb_matrix) + nb];
     int rows = vect[vect.size()  -nb_matrix +nb];
    int elts = col * rows; 
    std::vector<double> tmp;
    for(int i = 0; i < elts; i++)
    {
      if(i > 0 && i % col == 0)
      {
        mat.push_back(tmp);
        tmp.clear();
      }
      tmp.push_back(vect[old_pos + i]);
    }
    mat.push_back(tmp);
    old_pos+=elts;
    res.push_back(Matrix(mat));
   }
 return res;
}