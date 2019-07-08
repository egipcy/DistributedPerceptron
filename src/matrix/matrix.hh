#pragma once

#include <vector>

class Matrix
{
public:
  Matrix();
  Matrix(int rows, int columns, double init_value = 0);
  Matrix(std::vector<double> v);
  Matrix(std::vector<std::vector<double>> mat);

  void randomize();

  int rows() const;
  int columns() const; 
  std::vector<std::vector<double>>mat();
  std::vector<double>& operator[](int index);
  std::vector<double> operator[](int index) const;

  Matrix& operator+=(double n);
  Matrix& operator+=(const Matrix& other);
  Matrix operator+(double n) const;
  Matrix operator+(const Matrix& other) const;

  Matrix operator-() const;
  Matrix& operator-=(double n);
  Matrix& operator-=(const Matrix& other);
  Matrix operator-(double n) const;
  Matrix operator-(const Matrix& other) const;

  Matrix& operator*=(double n);
  Matrix& operator*=(const Matrix& other); // Hadamard product
  Matrix operator*(double n) const;
  Matrix operator*(const Matrix& other) const; // Hadamard product

  Matrix transpose() const;

  Matrix dot(const Matrix& other) const;

  Matrix map(double (*func) (double)) const;

  std::vector<double> to_vect() const;

  void print() const;
 
private:
  std::vector<std::vector<double>> mat_;
  int rows_;
  int columns_;
};

Matrix operator+(double n, const Matrix& other);
Matrix operator-(double n, const Matrix& other);
Matrix operator*(double n, const Matrix& other);

std::vector<Matrix> deserialize(std::vector<double> vect);
std::vector<double> serialize(std::vector<Matrix> Matrix);
