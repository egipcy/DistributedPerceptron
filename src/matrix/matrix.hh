#pragma once

#include <vector>

class Matrix
{
public:
  Matrix() = delete;
  Matrix(int rows, int columns, double init_value = 0);
  Matrix(std::vector<double> v);
  Matrix(int rows, int cols, std::vector<double> vect);
  void randomize();

  int rows() const;
  int columns() const;

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

private:
  std::vector<std::vector<double>> mat_;
  int rows_;
  int columns_;
};

Matrix operator+(double n, const Matrix& other);
Matrix operator-(double n, const Matrix& other);
Matrix operator*(double n, const Matrix& other);
