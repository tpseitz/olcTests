#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include "vector.h"

Matrix GetRotation(const Matrix&);

struct Triangle {
  int pi[3];
  int ni;
};

struct Line {
  int pi[2];
};

struct Mesh {
  std::vector<Vector> vertex;
  std::vector<Vector> normal;
  std::vector<Triangle> triangle;
  std::vector<Line> line;
};

struct Object {
private:
  Vector forward, up, right;
public:
  Mesh mesh;
  Vector location;
  Object();
  Object(Mesh, Vector);
  void MovePivot(const Vector&);
  void Roll(const double);
  void Yaw(const double);
  void Pitch(const double);
  void Forward(const double);
  void Up(const double);
  void Right(const double);
  void RotateX(const double);
  void RotateY(const double);
  void RotateZ(const double);
  Matrix GetTransformation();
  Matrix GetRotation();
};

extern const Mesh meshUnitCube;
extern const Mesh meshUtahTeapot;

Mesh LoadMesh(const std::string filename);

#endif
