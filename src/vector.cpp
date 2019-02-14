#include <cmath>
#include <cstring>
#include "vector.h"

Matrix Matrix::operator*(const Matrix &other) {
  Matrix tmp;
  for (int x = 0; x < 4; x ++)
    for (int y = 0; y < 4; y ++)
      for (int i = 0; i < 4; i++)
        tmp.m[x][y] += m[i][y] * other.m[x][i];
  return tmp;
}

void Matrix::operator*=(const Matrix &other) {
  double tmp[4][4] = { 0 };
  for (int x = 0; x < 4; x ++)
    for (int y = 0; y < 4; y ++)
      for (int i = 0; i < 4; i++)
        tmp[x][y] += m[i][y] * other.m[x][i];
  std::memcpy(&m, &tmp, 16);
}

/*
Vector Matrix::operator*(const Vector &vec) {
  return Vector(
    vec.x * m[0][0] + vec.y * m[1][0] + vec.z * m[2][0] + vec.m[3][0],
    vec.x * m[0][1] + vec.y * m[1][1] + vec.z * m[2][1] + vec.m[3][1],
    vec.x * m[0][2] + vec.y * m[1][2] + vec.z * m[2][2] + vec.m[3][2],
    vec.x * m[0][3] + vec.y * m[1][3] + vec.z * m[2][3] + vec.m[3][3]);
}
*/

Matrix Matrix::InversedCrude() {
  Matrix mat;
  mat.m[0][0] = m[0][0]; mat.m[0][1] = m[1][0]; mat.m[0][2] = m[2][0];
  mat.m[1][0] = m[0][1]; mat.m[1][1] = m[1][1]; mat.m[1][2] = m[2][1];
  mat.m[2][0] = m[0][2]; mat.m[2][1] = m[1][2]; mat.m[2][2] = m[2][2];

  mat.m[3][0] = -(m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0]
    + m[3][2] * mat.m[2][0]);
  mat.m[3][1] = -(m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1]
    + m[3][2] * mat.m[2][1]);
  mat.m[3][2] = -(m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2]
    + m[3][2] * mat.m[2][2]);
  mat.m[3][3] = 1.0f;

  return mat;
}

const Vector VECTOR_ILLEGAL = Vector(0, 1, 0);

Vector::Vector() { x = 0; y = 0; z = 0; w = 1; }

Vector::Vector(double x, double y, double z, double w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

Vector Vector::operator*(const double multiplier) {
  return Vector(x * multiplier, y * multiplier, z * multiplier);
}

void Vector::operator*=(const double multiplier) {
  x *= multiplier; y *= multiplier; z *= multiplier;
}

Vector Vector::operator/(const double divider) {
  return Vector(x / divider, y / divider, z / divider);
}

void Vector::operator/=(const double divider) {
  x /= divider; y /= divider; z /= divider;
}

Vector Vector::operator+(const Vector &other) {
  return Vector(x + other.x, y + other.y, z + other.z);
}

void Vector::operator+=(const Vector &other) {
  x += other.x; y += other.y; z += other.z;
}

Vector Vector::operator-(const Vector &other) {
  return Vector(x - other.x, y - other.y, z - other.z);
}

void Vector::operator-=(const Vector &other) {
  x -= other.x; y -= other.y; z -= other.z;
}

Vector Vector::operator*(const Matrix &mat) {
  return Vector(
    x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0],
    x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1],
    x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2],
    x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + mat.m[3][3]);
}

void Vector::operator*=(const Matrix &mat) {
  double tx, ty, tz;
  tx = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0];
  ty = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1];
  tz = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2];
  w  = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + mat.m[3][3];
  x = tx;
  y = ty;
  z = tz;
}

double Vector::length() {
  return std::sqrt(x * x + y * y + z * z);
}

Vector Vector::unit() {
  double l = std::sqrt(x * x + y * y + z * z);
  if (l == 0) return VECTOR_ILLEGAL;
  return *this / length();
}

Vector Vector::reversed() {
  return Vector(-x, -y, -z);
}

double Vector::dot(const Vector &other) {
  return x * other.x + y * other.y + z * other.z;
}

Vector Vector::cross(const Vector &other) {
  return Vector(
    y * other.z - z * other.y,
    z * other.x - x * other.z,
    x * other.y - y * other.x);
}

Vector Vector::rotatedX(const double angle_rad) {
  double sin = std::sin(angle_rad);
  double cos = std::cos(angle_rad);
  return Vector(x, y * cos - z * sin, y * sin + z * cos);
}

Vector Vector::rotatedY(const double angle_rad) {
  double sin = std::sin(angle_rad);
  double cos = std::cos(angle_rad);
  return Vector(x * cos - z * sin, y, x * sin + z * cos);
}

Vector Vector::rotatedZ(const double angle_rad) {
  double sin = std::sin(angle_rad);
  double cos = std::cos(angle_rad);
  return Vector(x * cos - y * sin, x * sin + y * cos, z);
}

