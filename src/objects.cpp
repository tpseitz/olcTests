#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "vector.h"
#include "objects.h"

Matrix GetRotation(const Matrix &matrix) {
  Matrix rot;
  memcpy(&rot.m[0][0], &matrix.m[0][0], 12 * sizeof(double));
  return rot;
}

Object::Object() {
  location = Vector( 0.0,  0.0,  0.0);
  forward  = Vector( 0.0,  0.0,  1.0);
  up       = Vector( 0.0, -1.0,  0.0);
  right    = Vector( 1.0,  0.0,  0.0);
}

Object::Object(Mesh mesh, Vector location) {
  this->mesh = mesh;
  this->location = location;
  forward  = Vector( 0.0,  0.0,  1.0);
  up       = Vector( 0.0, -1.0,  0.0);
  right    = Vector( 1.0,  0.0,  0.0);
}

void Object::MovePivot(const Vector& pivot) {
  for (Vector &v: mesh.vertex) v -= pivot;
}

/*
 * Roll, yaw and pitch uses Rodrigues' rotation formula to rotate
 */
void Object::Roll(const double rad) {
  // Rotate up and right using forward as axis
  double s = sin(rad), c = cos(rad);
  up = up * c + forward.cross(up) * s
    + forward * forward.dot(up) * (1 - c);
  right = right * c + forward.cross(right) * s
    + forward * forward.dot(right) * (1 - c);
}

void Object::Yaw(const double rad) {
  // Rotate forward and right using up as axis
  double s = sin(rad), c = cos(rad);
  forward = forward * c + up.cross(forward) * s
    + up * up.dot(forward) * (1 - c);
  right = right * c + up.cross(right) * s
    + up * up.dot(right) * (1 - c);
}

void Object::Pitch(const double rad) {
  // Rotate up and forward vectors by rad with right as axis
  double s = sin(rad), c = cos(rad);
  up = up * c + right.cross(up) * s
    + right * right.dot(up) * (1 - c);
  forward = forward * c + right.cross(forward) * s
    + right * right.dot(forward) * (1 - c);
}

/*
 * Axis rotation uses simple rotation matrixes for rotation
 */
void Object::RotateX(const double rad) {
  Matrix rot;
  rot.m[0][0] = 1;
  rot.m[1][1] =  cos(rad); rot.m[1][2] = -sin(rad);
  rot.m[2][1] =  sin(rad); rot.m[2][2] =  cos(rad);
  rot.m[3][3] = 1;

  up *= rot;
  forward *= rot;
  right *= rot;
}

void Object::RotateY(const double rad) {
  Matrix rot;
  rot.m[0][0] =  cos(rad); rot.m[0][2] =  sin(rad);
  rot.m[1][1] = 1;
  rot.m[2][0] = -sin(rad); rot.m[2][2] =  cos(rad);
  rot.m[3][3] = 1;

  up *= rot;
  forward *= rot;
  right *= rot;
}

void Object::RotateZ(const double rad) {
  Matrix rot;
  rot.m[0][0] =  cos(rad); rot.m[0][1] = -sin(rad);
  rot.m[1][0] =  sin(rad); rot.m[1][1] =  cos(rad);
  rot.m[2][2] = 1;
  rot.m[3][3] = 1;

  up *= rot;
  forward *= rot;
  right *= rot;
}

/*
 * Simple movement methods
 */
void Object::Forward(const double distance) {
  location += forward * distance;
}

void Object::Up(const double distance) {
  location += up * distance;
}

void Object::Right(const double distance) {
  location += right * distance;
}

/*
 * Transformation and rotation matrixes
 */
Matrix Object::GetTransformation() {
  Matrix mat;
  memcpy(&mat.m[0][0], &right,    3 * sizeof(double));
  memcpy(&mat.m[1][0], &up,       3 * sizeof(double));
  memcpy(&mat.m[2][0], &forward,  3 * sizeof(double));
  memcpy(&mat.m[3][0], &location, 4 * sizeof(double));
  return mat;
}

Matrix Object::GetRotation() {
  Matrix mat;
  memcpy(&mat.m[0][0], &right,    3 * sizeof(double));
  memcpy(&mat.m[1][0], &up,       3 * sizeof(double));
  memcpy(&mat.m[2][0], &forward,  3 * sizeof(double));
//  mat.m[3][3] = 1.0;
  return mat;
}

const Mesh meshUnitCube = {
  {
    { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 0, 0, 1 },
    { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 1, 0, 1 }
  }, {
    { -1,  0,  0 }, { -1,  0,  0 }, {  0, -1, -0 }, {  0, -1,  0 },
    {  1,  0,  0 }, {  1,  0,  0 }, {  0,  1,  0 }, {  0,  1, -0 },
    {  0,  0, -1 }, { -0,  0, -1 }, {  0,  0,  1 }, { -0,  0,  1 }
  }, {
    { 3, 0, 1,  0 }, { 1, 2, 3,  1 }, { 7, 4, 0,  2 }, { 0, 3, 7,  3 },
    { 6, 5, 4,  4 }, { 4, 7, 6,  5 }, { 2, 1, 5,  6 }, { 5, 6, 2,  7 },
    { 0, 4, 5,  8 }, { 5, 1, 0,  9 }, { 7, 3, 2, 10 }, { 2, 6, 7, 11 }
  }, {}
};

const Mesh meshUtahTeapot = {
  {
    {  0.2000,  0.0000, 2.70000 }, {  0.2000, -0.1120, 2.70000 },
    {  0.1120, -0.2000, 2.70000 }, {  0.0000, -0.2000, 2.70000 },
    {  1.3375,  0.0000, 2.53125 }, {  1.3375, -0.7490, 2.53125 },
    {  0.7490, -1.3375, 2.53125 }, {  0.0000, -1.3375, 2.53125 },
    {  1.4375,  0.0000, 2.53125 }, {  1.4375, -0.8050, 2.53125 },
    {  0.8050, -1.4375, 2.53125 }, {  0.0000, -1.4375, 2.53125 },
    {  1.5000,  0.0000, 2.40000 }, {  1.5000, -0.8400, 2.40000 },
    {  0.8400, -1.5000, 2.40000 }, {  0.0000, -1.5000, 2.40000 },
    {  1.7500,  0.0000, 1.87500 }, {  1.7500, -0.9800, 1.87500 },
    {  0.9800, -1.7500, 1.87500 }, {  0.0000, -1.7500, 1.87500 },
    {  2.0000,  0.0000, 1.35000 }, {  2.0000, -1.1200, 1.35000 },
    {  1.1200, -2.0000, 1.35000 }, {  0.0000, -2.0000, 1.35000 },
    {  2.0000,  0.0000, 0.90000 }, {  2.0000, -1.1200, 0.90000 },
    {  1.1200, -2.0000, 0.90000 }, {  0.0000, -2.0000, 0.90000 },
    { -2.0000,  0.0000, 0.90000 }, {  2.0000,  0.0000, 0.45000 },
    {  2.0000, -1.1200, 0.45000 }, {  1.1200, -2.0000, 0.45000 },
    {  0.0000, -2.0000, 0.45000 }, {  1.5000,  0.0000, 0.22500 },
    {  1.5000, -0.8400, 0.22500 }, {  0.8400, -1.5000, 0.22500 },
    {  0.0000, -1.5000, 0.22500 }, {  1.5000,  0.0000, 0.15000 },
    {  1.5000, -0.8400, 0.15000 }, {  0.8400, -1.5000, 0.15000 },
    {  0.0000, -1.5000, 0.15000 }, { -1.6000,  0.0000, 2.02500 },
    { -1.6000, -0.3000, 2.02500 }, { -1.5000, -0.3000, 2.25000 },
    { -1.5000,  0.0000, 2.25000 }, { -2.3000,  0.0000, 2.02500 },
    { -2.3000, -0.3000, 2.02500 }, { -2.5000, -0.3000, 2.25000 },
    { -2.5000,  0.0000, 2.25000 }, { -2.7000,  0.0000, 2.02500 },
    { -2.7000, -0.3000, 2.02500 }, { -3.0000, -0.3000, 2.25000 },
    { -3.0000,  0.0000, 2.25000 }, { -2.7000,  0.0000, 1.80000 },
    { -2.7000, -0.3000, 1.80000 }, { -3.0000, -0.3000, 1.80000 },
    { -3.0000,  0.0000, 1.80000 }, { -2.7000,  0.0000, 1.57500 },
    { -2.7000, -0.3000, 1.57500 }, { -3.0000, -0.3000, 1.35000 },
    { -3.0000,  0.0000, 1.35000 }, { -2.5000,  0.0000, 1.12500 },
    { -2.5000, -0.3000, 1.12500 }, { -2.6500, -0.3000, 0.93750 },
    { -2.6500,  0.0000, 0.93750 }, { -2.0000, -0.3000, 0.90000 },
    { -1.9000, -0.3000, 0.60000 }, { -1.9000,  0.0000, 0.60000 },
    {  1.7000,  0.0000, 1.42500 }, {  1.7000, -0.6600, 1.42500 },
    {  1.7000, -0.6600, 0.60000 }, {  1.7000,  0.0000, 0.60000 },
    {  2.6000,  0.0000, 1.42500 }, {  2.6000, -0.6600, 1.42500 },
    {  3.1000, -0.6600, 0.82500 }, {  3.1000,  0.0000, 0.82500 },
    {  2.3000,  0.0000, 2.10000 }, {  2.3000, -0.2500, 2.10000 },
    {  2.4000, -0.2500, 2.02500 }, {  2.4000,  0.0000, 2.02500 },
    {  2.7000,  0.0000, 2.40000 }, {  2.7000, -0.2500, 2.40000 },
    {  3.3000, -0.2500, 2.40000 }, {  3.3000,  0.0000, 2.40000 },
    {  2.8000,  0.0000, 2.47500 }, {  2.8000, -0.2500, 2.47500 },
    {  3.5250, -0.2500, 2.49375 }, {  3.5250,  0.0000, 2.49375 },
    {  2.9000,  0.0000, 2.47500 }, {  2.9000, -0.1500, 2.47500 },
    {  3.4500, -0.1500, 2.51250 }, {  3.4500,  0.0000, 2.51250 },
    {  2.8000,  0.0000, 2.40000 }, {  2.8000, -0.1500, 2.40000 },
    {  3.2000, -0.1500, 2.40000 }, {  3.2000,  0.0000, 2.40000 },
    {  0.0000,  0.0000, 3.15000 }, {  0.8000,  0.0000, 3.15000 },
    {  0.8000, -0.4500, 3.15000 }, {  0.4500, -0.8000, 3.15000 },
    {  0.0000, -0.8000, 3.15000 }, {  0.0000,  0.0000, 2.85000 },
    {  1.4000,  0.0000, 2.40000 }, {  1.4000, -0.7840, 2.40000 },
    {  0.7840, -1.4000, 2.40000 }, {  0.0000, -1.4000, 2.40000 },
    {  0.4000,  0.0000, 2.55000 }, {  0.4000, -0.2240, 2.55000 },
    {  0.2240, -0.4000, 2.55000 }, {  0.0000, -0.4000, 2.55000 },
    {  1.3000,  0.0000, 2.55000 }, {  1.3000, -0.7280, 2.55000 },
    {  0.7280, -1.3000, 2.55000 }, {  0.0000, -1.3000, 2.55000 },
    {  1.3000,  0.0000, 2.40000 }, {  1.3000, -0.7280, 2.40000 },
    {  0.7280, -1.3000, 2.40000 }, {  0.0000, -1.3000, 2.40000 }
  }, {}, {}, {
    { 102, 103 }, { 103, 104 }, { 104, 105 }, { 105, 4 }, { 4, 5 }, { 5, 6 },
    { 6, 7 }, { 7, 8 }, { 8, 9 }, { 9, 10 }, { 10, 11 }, { 11, 12 },
    { 12, 13 }, { 13, 14 }, { 14, 15 }, // Rim
    { 12, 13 }, { 13, 14 }, { 14, 15 }, { 15, 16 }, { 16, 17 }, { 17, 18 },
    { 18, 19 }, { 19, 20 }, { 20, 21 }, { 21, 22 }, { 22, 23 }, { 23, 24 },
    { 24, 25 }, { 25, 26 }, { 26, 27 }, // Body 1
    { 24, 25 }, { 25, 26 }, { 26, 27 }, { 27, 29 }, { 29, 30 }, { 30, 31 },
    { 31, 32 }, { 32, 33 }, { 33, 34 }, { 34, 35 }, { 35, 36 }, { 36, 37 },
    { 37, 38 }, { 38, 39 }, { 39, 40 }, // Body 2
    { 96, 96 }, { 96, 96 }, { 96, 96 }, { 96, 97 }, { 97, 98 }, { 98, 99 },
    { 99, 100 }, { 100, 101 }, { 101, 101 }, { 101, 101 }, { 101, 101 },
    { 101, 0 }, { 0, 1 }, { 1, 2 }, { 2, 3 }, // Lid 1
    { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 106 }, { 106, 107 }, { 107, 108 },
    { 108, 109 }, { 109, 110 }, { 110, 111 }, { 111, 112 }, { 112, 113 },
    { 113, 114 }, { 114, 115 }, { 115, 116 }, { 116, 117 }, // Lid 2
    { 41, 42 }, { 42, 43 }, { 43, 44 }, { 44, 45 }, { 45, 46 }, { 46, 47 },
    { 47, 48 }, { 48, 49 }, { 49, 50 }, { 50, 51 }, { 51, 52 }, { 52, 53 },
    { 53, 54 }, { 54, 55 }, { 55, 56 }, // Handle 1
    { 53, 54 }, { 54, 55 }, { 55, 56 }, { 56, 57 }, { 57, 58 }, { 58, 59 },
    { 59, 60 }, { 60, 61 }, { 61, 62 }, { 62, 63 }, { 63, 64 }, { 64, 28 },
    { 28, 65 }, { 65, 66 }, { 66, 67 }, // Handle 2
    { 68, 69 }, { 69, 70 }, { 70, 71 }, { 71, 72 }, { 72, 73 }, { 73, 74 },
    { 74, 75 }, { 75, 76 }, { 76, 77 }, { 77, 78 }, { 78, 79 }, { 79, 80 },
    { 80, 81 }, { 81, 82 }, { 82, 83 }, // Spout 1
    { 80, 81 }, { 81, 82 }, { 82, 83 }, { 83, 84 }, { 84, 85 }, { 85, 86 },
    { 86, 87 }, { 87, 88 }, { 88, 89 }, { 89, 90 }, { 90, 91 }, { 91, 92 },
    { 92, 93 }, { 93, 94 }, { 94, 95 } // Spout 2
  }
};

int BUFFER_SIZE_READ = 1024;
Mesh LoadMesh(const std::string filename) {
  Mesh tmp;

  std::vector<Vector> surface_normal;

  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Could not open file " << filename << std::endl;
    return tmp;
  }

  while (!f.eof()) {
    char line[BUFFER_SIZE_READ];
    f.getline(&line[0], BUFFER_SIZE_READ);

    std::stringstream s;
    s << line;

    char c;
    if (line[0] == '\x00') continue; // Skip empty lines
    if (line[0] == '#') continue; // Skip connect lines
    if (line[0] == 'v') {
      if (line[1] == ' ') {
        Vector v;
        s >> c >> v.x >> v.y >> v.z;
        tmp.vertex.push_back(v);
      } else if (line[1] == 'n') {
        Vector v;
        s >> c >> c >> v.x >> v.y >> v.z;
        tmp.normal.push_back(v);
      }
    } else if (line[0] == 'f' and line[1] == ' ') {
      int il[10] = { 0 }, ii = 0;
      char c;
      for (int i = 2; i < BUFFER_SIZE_READ && line[i] != 0; i++) {
        c = line[i];
        if (c >= '0' and c <= '9') il[ii] = il[ii] * 10 + c - '0';
        if (c == '/' or c == ' ') ii++;
//        std::cout << il[ii] << ' '; //XXX
      }
//      std::cout << ii << std::endl; //XXX

      int i0 = 0, i1 = 0, i2 = 0, n0 = 0, n1 = 0, n2 = 0, sn = -1;
      if (ii == 2) {
        i0 = il[0]; i1 = il[1]; i2 = il[2];
      } else if (ii == 5) {
        i0 = il[0]; i1 = il[2]; i2 = il[4];
      } else if (ii == 8) {
        i0 = il[0]; i1 = il[3]; i2 = il[6];
        n0 = il[2]; n1 = il[5]; n2 = il[8];
      } else {
        std::cerr << "Unknown face line: " << ii << "\n  " << line << std::endl;
        continue;
      }

      if (n0 >= 0 && n0 < tmp.normal.size()
         && n1 >= 0 && n1 < tmp.normal.size()
         && n2 >= 0 && n2 < tmp.normal.size()
      ) {
        sn = surface_normal.size();
        surface_normal.push_back(
          (tmp.normal[n0] + tmp.normal[n0] + tmp.normal[n0]) / 3);
      }

      i0--; i1--; i2--; // Correct the obj file indexing
      if (i0 >= 0 && i0 < tmp.vertex.size()
         && i1 >= 0 && i1 < tmp.vertex.size()
         && i2 >= 0 && i2 < tmp.vertex.size()
      ) {
          tmp.triangle.push_back({ i0, i1, i2, sn });
      } else {
        std::cout << "Illegal face: "
          << i0 << ',' << i1 << ',' << i2 << std::endl;
      }
//      std::cout << i0 << ',' << i1 << ',' << i2 << std::endl; //XXX
    } else {
      std::cerr << "Unknown line: " << line << std::endl; //XXX
    }
  }

  f.close();

  //<XXX>
  tmp.normal.clear();
  if (surface_normal.size() > 0) {
    for (Vector v: surface_normal) tmp.normal.push_back(v);
    surface_normal.clear();
  } //</XXX>

  if (tmp.vertex.size() > 0 && tmp.normal.size() == 0) {
    for (Triangle &t: tmp.triangle) {
      Vector n = (tmp.vertex[t.pi[1]] - tmp.vertex[t.pi[0]]).cross(
        tmp.vertex[t.pi[2]] - tmp.vertex[t.pi[0]]).unit();
      int index = tmp.normal.size();
      // TODO Check for duplicate normals
      t.ni = index;
      tmp.normal.push_back(n);
    }
  }

  std::cout << "Succesfully loaded model " << filename << std::endl; //XXX

  return tmp;
}
