#ifndef VECTOR_H
#define VECTOR_H

struct Matrix {
  double m[4][4] { 0 };
  Matrix operator*(const Matrix&);
  void operator*=(const Matrix&);
  Matrix InversedCrude();
};

struct Vector {
//  double m[4];
  double x, y, z, w;

  Vector();
  Vector(double, double, double = 0.0, double = 1.0);

  Vector operator*(const double);
  void operator*=(const double);
  void operator*=(const Matrix&);
  Vector operator*(const Matrix&);
  Vector operator/(const double);
  void operator/=(const double);
  Vector operator+(const Vector&);
  void operator+=(const Vector&);
  Vector operator-(const Vector&);
  void operator-=(const Vector&);

  double length();
  Vector unit();
  Vector reversed();
  double dot(const Vector&);
  Vector cross(const Vector&);
  Vector rotatedX(const double);
  Vector rotatedY(const double);
  Vector rotatedZ(const double);
};

#endif
