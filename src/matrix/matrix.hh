#pragma once

class Matrix
{
public:
  Matrix(int rows, int columns, bool random = false, double init_value = 0);

  int rows() const;
  int columns() const;

  std::vector<double>& operator[](int index);
  std::vector<double> operator[](int index) const;

  Matrix& operator+=(double n);
  Matrix& operator+=(const Matrix& other);
  Matrix operator+(double n) const;
  Matrix operator+(const Matrix& other) const;

  Matrix& operator*=(double n);
  Matrix& operator*=(const Matrix& other); // Hadamard product
  Matrix operator*(double n) const;
  Matrix operator*(const Matrix& other) const; // Hadamard product

  Matrix dot(const Matrix& other) const;

  Matrix map(const double (*func) (double)&) const;

private:
  std::vector<std::vector<double>> mat_;
  int rows_;
  int columns_;
};