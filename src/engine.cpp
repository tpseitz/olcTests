#include <algorithm>
#include "engine.h"

const int WIDTH = 480, HEIGHT = 270, PIXEL_SIZE = 1;
//const int WIDTH = 960, HEIGHT = 540, PIXEL_SIZE = 1;
//const int WIDTH = 1920, HEIGHT = 1080, PIXEL_SIZE = 1;
olc::Pixel COLOR_BACKGROUND = olc::Pixel(0, 0, 0);
olc::Pixel COLOR_WIREFRAME = olc::Pixel(32, 32, 32);
olc::Pixel COLOR_VERTEX = olc::Pixel(255, 255, 255);
olc::Pixel COLOR_GRID = olc::Pixel(192, 0, 0);
bool SHOW_RULER = false;
bool SHOW_WIREFRAME = false;
bool SHADING = true;

const double tau = 6.283185307179586;
const double pi  = 3.141592653589793;

struct ProjectedTriangle {
  Vector p[3];
  olc::Pixel color = olc::Pixel(192, 192, 192);
};

Engine *Engine::current = NULL;

bool Engine::compareObjects(Object *obj1, Object *obj2) {
  return (obj1->location - current->camera.location).length()
    > (obj2->location - current->camera.location).length();
}

Engine::Engine() {
  current = this;
  sAppName = "3D engine test";
  cenX = WIDTH / 2;
  cenY = HEIGHT / 2;
}

bool Engine::OnUserCreate() {
  cube = new Object(meshUnitCube, Vector(0.0, 0.0, 2.5));
  cube->MovePivot(Vector(0.5, 0.5, 0.5));
  entities.push_back(cube);

//  ship = new Object(LoadMesh("VideoShip.obj"), Vector(-8.0, 0.0, 9.0));
  ship = new Object(LoadMesh("example_ship.obj"), Vector(-8.0, 0.0, 9.0));
  ship->MovePivot(Vector(0.0, 0.0, 1.0));
  entities.push_back(ship);

  running = true;
  return true;
}

void Engine::DisplayTriangle(
    const Vector p1, const Vector p2, const Vector p3, double dLight) {
  int c = 192;
  if (SHADING) {
    if (dLight < -1) dLight = -1;
    else if (dLight > 1) dLight = 1;
    c = (int)(128.0 + dLight * 96.0);
  }
  olc::Pixel col = olc::Pixel(c, c, c);
  FillTriangle(
    (int)(cenX + p1.x * cenX), (int)(cenY + p1.y * cenY),
    (int)(cenX + p2.x * cenX), (int)(cenY + p2.y * cenY),
    (int)(cenX + p3.x * cenX), (int)(cenY + p3.y * cenY),
    col);
  if (SHOW_WIREFRAME) DrawTriangle(
    (int)(cenX + p1.x * cenX), (int)(cenY + p1.y * cenY),
    (int)(cenX + p2.x * cenX), (int)(cenY + p2.y * cenY),
    (int)(cenX + p3.x * cenX), (int)(cenY + p3.y * cenY),
    COLOR_WIREFRAME);
}

bool Engine::OnUserUpdate(float fElapsedTime) {
  dTime += fElapsedTime;

  // ESC will exit the application
  if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

  // WASD moves the camera, arrow keys turn, up, spacebar moves up, control
  // -key moves down and Q and E does roll the camera
  if (GetKey(olc::Key::A).bHeld)
    camera.Right(-fElapsedTime * dSpeed);
  if (GetKey(olc::Key::D).bHeld)
    camera.Right(fElapsedTime * dSpeed);
  if (GetKey(olc::Key::W).bHeld)
    camera.Forward(fElapsedTime * dSpeed);
  if (GetKey(olc::Key::S).bHeld)
    camera.Forward(-fElapsedTime * dSpeed);
  if (GetKey(olc::Key::Q).bHeld)
    camera.Roll(fElapsedTime);
  if (GetKey(olc::Key::E).bHeld)
    camera.Roll(-fElapsedTime);
  if (GetKey(olc::Key::SPACE).bHeld)
    camera.Up(-fElapsedTime * dSpeed);
  if (GetKey(olc::Key::CTRL).bHeld)
    camera.Up(fElapsedTime * dSpeed);
  if (GetKey(olc::Key::UP).bHeld)
    camera.Pitch(-fElapsedTime);
  if (GetKey(olc::Key::DOWN).bHeld)
    camera.Pitch(fElapsedTime);
  if (GetKey(olc::Key::LEFT).bHeld)
    camera.Yaw(fElapsedTime);
  if (GetKey(olc::Key::RIGHT).bHeld)
    camera.Yaw(-fElapsedTime);

  // Object rotation
  if (cube != NULL) {
    cube->RotateX(fElapsedTime / 4.0);
    cube->RotateY(fElapsedTime / 2.0);
    cube->RotateZ(fElapsedTime);
  }

  if (ship != NULL) {
    ship->RotateX(fElapsedTime / 4.0);
    ship->RotateY(fElapsedTime / 2.0);
    ship->RotateZ(fElapsedTime);
  }

  // Calculate projection matrix
  dAspectRatio = (double)ScreenHeight() / (double)ScreenWidth();
  double dFovTan = 1.0 / tan(dFovDegrees / 360.0 * pi);

  matProj.m[0][0] = dAspectRatio * dFovTan;
  matProj.m[1][1] = dFovTan;
  matProj.m[2][2] = dFar / (dFar - dNear);
  matProj.m[3][2] = (-dFar * dNear) / (dFar - dNear);
  matProj.m[2][3] = 1.0;

  // Calculate world transformation matrix
  Matrix matWrldTrn = camera.GetTransformation().InversedCrude();
  Matrix matWrldRot = GetRotation(matWrldTrn);

  // Paint background color
  FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_BACKGROUND);
  // Update screen center
  cenX = ScreenWidth() / 2, cenY = ScreenHeight() / 2;

  std::sort(entities.begin(), entities.end(), compareObjects);
  for (Object *obj: entities) {
    // Create transformation matrix from object
    Matrix matTrn = obj->GetTransformation();
    Matrix matRot = GetRotation(matTrn);
    // Create list of rotated and transformed vertices
    std::vector<Vector> mv;
    for (Vector v: obj->mesh.vertex) mv.push_back((v * matTrn) * matWrldTrn);
    // Create list of rotated normals
    std::vector<Vector> mn;
    for (Vector v: obj->mesh.normal) mn.push_back((v * matRot) * matWrldRot);

    // Create list of visible triangles
    std::vector<Triangle> tt;
    for (Triangle tri: obj->mesh.triangle)
      if (mv[tri.pi[0]].z > dNear) //XXX
      if (mn[tri.ni].dot(mv[tri.pi[0]]) < 0)
        tt.push_back(tri);

    // Project vertices
    Vector vec;
    std::vector<Vector> pt;
    for (Vector v: mv) {
      vec = v;
      vec *= matProj;
      if (vec.w != 0.0) vec /= vec.w;
      pt.push_back(vec);
    }

    // Sort projected triangles by z coordinate
    std::sort(tt.begin(), tt.end(), [&pt](Triangle &t1, Triangle &t2) {
      double cz1 = (pt[t1.pi[0]].z + pt[t1.pi[1]].z + pt[t1.pi[2]].z) / 3;
      double cz2 = (pt[t2.pi[0]].z + pt[t2.pi[1]].z + pt[t2.pi[2]].z) / 3;
      return cz1 > cz2;
    });

    // Draw triangles
    double la;
    for (Triangle tri: tt) {
      la = mn[tri.ni].dot(vecWorldLight);
      DisplayTriangle(pt[tri.pi[0]], pt[tri.pi[1]], pt[tri.pi[2]], la);//XXX
    }

    for (Line l: obj->mesh.line) {
      int x1 =(int)(cenX + pt[l.pi[0]].x * cenX);
      int y1 = (int)(cenY + pt[l.pi[0]].y * cenY);
      int x2 =(int)(cenX + pt[l.pi[1]].x * cenX);
      int y2 = (int)(cenY + pt[l.pi[1]].y * cenY);
      DrawLine(x1, y1, x2, y2, COLOR_VERTEX);
    }

    if (obj->mesh.triangle.size() == 0 && obj->mesh.line.size() == 0) {
      for (Vector p: pt) Draw(
        (int)(cenX + p.x * cenX), (int)(cenY + p.y * cenY), COLOR_VERTEX);
    }
  }

  // GUI stuff
  if (SHOW_RULER) {
    DrawLine(cenX, 0, cenX, ScreenHeight(), COLOR_GRID);
    DrawLine(0, cenY, ScreenWidth(), cenY, COLOR_GRID);
  }

//  return false; //XXX
  return running;
}

Engine::~Engine() {
  for (Object* o: entities) delete o;
}

int main() {
  Engine eng;
  if (eng.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) eng.Start();

  return 0;
}

