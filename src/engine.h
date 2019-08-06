#include "olcPixelGameEngine.h"
#include "vector.h"
#include "objects.h"

class Engine : public olc::PixelGameEngine {
private:
  static Engine *current;

  bool running = false;
  double dTime = 0;
  double cenX = 400, cenY = 300;
  double dAspectRatio = 1;
  Matrix matProj = { 0 };

  static bool compareObjects(Object*, Object*);

public:
  double dFovDegrees = 70.0;
  double dNear = 1.0, dFar = 1000.0; // dWhereverYouAre
  double dSpeed = 3.0;
  Vector vecWorldLight = Vector(-1, -1, 0).unit();
  Object camera;
  std::vector<Object*> entities;
  Object *cube = NULL, *ship = NULL;

  Engine();
  bool OnUserCreate() override;
  void DisplayTriangle(const Vector, const Vector, const Vector, double);
  bool OnUserUpdate(float) override;
  ~Engine();
};

