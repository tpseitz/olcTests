#include "olcPixelGameEngine.h"

const int WIDTH = 960, HEIGHT = 540, PIXEL_SIZE = 1;
const int COLORS = 8;
const float GRAVITY = 7.0;

struct Line { int x1, y1, x2, y2; };
struct Point { int x, y; };

olc::Pixel COLOR_NIGHT = olc::Pixel(  0,   0,   0);
olc::Pixel COLOR_LIGHT = olc::Pixel(255, 255, 255);
olc::Pixel COLOR_MAP[COLORS] = {
  { 255,   0,   0 },
  {   0, 255,   0 },
  {   0,   0, 255 },
  { 255, 255,   0 },
  { 255,   0, 255 },
  {   0, 255, 255 },
  { 255, 255, 255 },
  { 255, 128,   0 },
};

struct Particle {
  float x = 0.0, y = 0.0, sx = 0.0, sy = -0.1, age = 0.0;
  int lvl = 1;
  bool alive = true;

  Particle(int _x, int _y, int _sx, int _sy, int _lvl) {
    x = _x; y = _y;
    sx = _sx; sy = _sy;
    lvl = _lvl;
    age = float(rand() % 1000) / 1000.0;
  }
};

class Fireworks : public olc::PixelGameEngine {
public:
  float fTime = 0.0, fNext = 0.0;
  bool running = true;
  std::vector<Particle*> particles;

  Fireworks() {
    sAppName = "Fireworks";
  }

  bool OnUserCreate() override {
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;
    bool explosion = false;

    if (fNext < fTime) {
      fNext = fTime + 0.5 + (float)(rand() % 200) / 100.0;
      int level = 1;
      if (rand() % 5 == 0) level = 2;
      Particle* p = new Particle(
        float(ScreenWidth() / 4 + rand() % (ScreenWidth() / 2)),
        float(ScreenHeight()),
        float(rand() % 20000 - 10000) /  1000.0,
        float(rand() % 25000 + 25000) / -500.0,
        level
      );
      particles.push_back(p);
    }

    float width = float(ScreenWidth());
    float height = float(ScreenHeight());

    std::vector<Particle*> tmp;
    for (Particle* p: particles) {
      p->age += fElapsedTime;
      p->sy += fElapsedTime * GRAVITY;
      p->x += p->sx * fElapsedTime;
      p->y += p->sy * fElapsedTime;

      if (p->x < 0 or p->x > width or p->y > height) p->alive = false;
      if (p->lvl > 0) {
        if (p->sy > 0 and p->age > 3.0 and p->y > -5 and p->y > -5.0) {
          explosion = true;
          int max = 25 + (rand() % 75);
          if (p->lvl > 1) max = 5 + (rand() % 10);
          for (int i = 0; i < max; i++) {
            float a = float(rand() % 314159) / 50000.0;
            float d = float(rand() % 5000) / 100.0;
            float sx = std::sin(a) * d, sy = std::cos(a) * d;
            Particle* pp = new Particle(
              p->x, p->y, p->sx + sx, p->sy + sy, p->lvl - 1);
            tmp.push_back(pp);
          }
          p->alive = false;
        }
      } else {
        if (p->age > 5.0) p->alive = false;
      }

      if (p->alive) tmp.push_back(p);
      else delete p;
    }
    particles.clear();
    particles = tmp;

    FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_NIGHT);
//    if (explosion) {
//      FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_LIGHT);
//    } else {
//      FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_NIGHT);
//    }

    for (Particle* p: particles)
      Draw(int(p->x), int(p->y), COLOR_MAP[rand() % COLORS]);

    if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

    return running;
  }
};

int main() {
  Fireworks scr;
  if (scr.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) scr.Start();
  return 0;
}
