#include "olcPixelGameEngine.h"
#include "vector.h"

//const int WIDTH = 960, HEIGHT = 540, PIXEL_SIZE = 1;
const int WIDTH = 480, HEIGHT = 270, PIXEL_SIZE = 1;
const olc::Pixel COLOR_BACKGROUND = olc::Pixel(0, 0, 0);

Vector GRAVITY = Vector(0.0, 9.0);
double SUCKING = 0.06;
double RESISTANCE = 0.2;
double MIN_VELOCITY = 5.0;
double VELOCITY_VAR = 10.0;
double MAX_AGE = 8.0;
double WIND = 5.0;
double PARTICLES_PER_S = 500.0;
double RADIUS = 5.0;
double ALPHA = 0.75;

olc::Sprite *gfxFire; 

enum ParticleType { NONE, FIRE };
struct Particle {
  ParticleType type = NONE;
  Vector location;
  Vector force;
  double age = 0;
  Vector* parent = NULL;
};

double rnd() {
  return (float)(rand() % RAND_MAX) / (float)RAND_MAX;
}

void createSprites() {
  float rad;
  int r = 255, g = 64, b = 8, a = 0;
  gfxFire = new olc::Sprite(RADIUS * 2 + 1, RADIUS * 2 + 1);
  for (int x = -RADIUS; x < RADIUS + 1; x++) {
    for (int y = -RADIUS; y < RADIUS + 1; y++) {
      rad = std::sqrt(x * x + y * y);
      if (rad > RADIUS) a = 0;
      else a = (int)((RADIUS - rad) / RADIUS * ALPHA * 255.0);
//      std::cout << a << ' '; //XXX
      gfxFire->SetPixel(
        (int)(RADIUS + x), (int)(RADIUS + y),
        olc::Pixel(r, g, b, a));
    }
//    std::cout << std::endl; //XXX
  }
}

double dBlendFactor = 1.0;

olc::Pixel ModePixelAdd(
  const int x, const int y, const olc::Pixel& p1, const olc::Pixel& p2
) {
  int r = p1.r + p2.r;
  if (r > 255) r = 255;
  int g = p1.g + p2.g;
  if (g > 255) g = 255;
  int b = p1.b + p2.b;
  if (b > 255) b = 255;
  int a = p1.a + p2.a;
  if (a > 255) a = 255;

  return olc::Pixel(r, g, b, a);
}

olc::Pixel ModePixelAlphaAdd(
  const int x, const int y, const olc::Pixel& p, const olc::Pixel& target
) {
  int a = target.a + (int)((double)p.a * dBlendFactor);
  if (a > 255) a = 255;

  float aa = (float)p.a / 255.0f * dBlendFactor;

  float r = target.r + (float)p.r * aa;
  if (r > 255) r = 255;
  float g = target.g + (float)p.g * aa;
  if (g > 255) g = 255;
  float b = target.b + (float)p.b * aa;
  if (b > 255) b = 255;

  return olc::Pixel((int)r, (int)g, (int)b, a);
}

class MyApp : public olc::PixelGameEngine {
public:
  float fTime = 0.0;
  std::vector<Particle> fire;
  Vector fireCenter, wind;
  bool running = true;

  MyApp() {
    sAppName = "Tail";
  }

  bool OnUserCreate() override {
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;

    if (fireCenter.x == 0)
      fireCenter = Vector(ScreenWidth() / 2, ScreenHeight() * 9 / 10);

    FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_BACKGROUND);

    int pc = (int)(fElapsedTime * PARTICLES_PER_S);
    for (int i = 0; i < pc; i++) {
      float a = rnd() * 6.28318f;
      float v = rnd() * VELOCITY_VAR +  MIN_VELOCITY;
      Vector force = Vector(std::sin(a) * v, std::cos(a) * v);
      double sa = rnd() * 0.5 - 0.1;
      Particle p = { FIRE, fireCenter, force, sa, &fireCenter };
      p.location.x += rnd() * 10.0 - 10.0;
      fire.push_back(p);
    }

    wind.x = std::sin(fTime * 5.0) * WIND;

    float res = 1.0f - RESISTANCE * fElapsedTime;
    float suc = fElapsedTime * SUCKING;
    Vector grav = GRAVITY * fElapsedTime;
    Vector wd = wind * fElapsedTime;
    Vector sv;
    std::vector<Particle> tmp;
    for (Particle p: fire) {
      sv = (p.location - *p.parent) * suc;
      sv.y = 0;
      p.force = p.force * res - sv + wd - grav;

      p.location += p.force * fElapsedTime;

      p.age += fElapsedTime;
      if (p.age < MAX_AGE) tmp.push_back(p);
    }
    fire.clear();
    fire = tmp;

//    SetPixelMode(olc::Pixel::ALPHA);
    SetPixelMode(ModePixelAlphaAdd);
//    DrawSprite(5, 5, gfxFire); //XXX
    for (Particle p: fire) {
      dBlendFactor =( 1.0 - p.age / MAX_AGE) * 1.5;
      DrawSprite((int)(p.location.x), (int)(p.location.y), gfxFire);
    }
    SetPixelMode(olc::Pixel::NORMAL);

    if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

    return running;
  }
};

int main() {
  createSprites();

  MyApp scr;
  if (scr.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) scr.Start();

  return 0;
}
