#include "olcPixelGameEngine.h"

const int WIDTH = 960, HEIGHT = 540, PIXEL_SIZE = 1;
const int ROUNDS = 5;

struct Line { int x1, y1, x2, y2; };
struct Point { int x, y; };

olc::Pixel COLOR_NIGHT = olc::Pixel(0, 0, 0);
olc::Pixel COLOR_LIGHT = olc::Pixel(255, 255, 255);
olc::Pixel COLOR_LIGHTNING = olc::Pixel(192, 192, 255);

class MyApp : public olc::PixelGameEngine {
public:
  float fTime = 0.0, fNext = 0.0;
  bool running = true;
  std::vector<Line> lines;
  std::vector<Point> ends;

  MyApp() {
    sAppName = "Lightnings";
  }

  bool OnUserCreate() override {
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;

    for (int i = 0; i < ROUNDS; i++) {
      if (ends.size() == 0) break;
      std::vector<Point> tmp;
      Point p2;
      for (Point p1: ends) {
        p2 = { p1.x + rand() % 16 - 8, p1.y + rand() % 30 };
        lines.push_back({ p1.x, p1.y, p2.x, p2.y });
        if (p2.y < ScreenHeight()) tmp.push_back(p2);
        if (ends.size() < 5 && rand() % 15 < 3 - ends.size()) {
          p2 = { p1.x + rand() % 16 - 8, p1.y + rand() % 30 };
          lines.push_back({ p1.x, p1.y, p2.x, p2.y });
          if (p2.y < ScreenHeight()) tmp.push_back(p2);
        }
      }
      ends = tmp;
    }

    if (lines.size() > 0) {
      if (ends.size() == 0) {
        lines.clear();
        fNext = fTime + 0.5 + (float)(rand() % 200) / 100.0;
      }
      FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_NIGHT);
      for (Line l: lines) DrawLine(l.x1, l.y1, l.x2, l.y2, COLOR_LIGHTNING);
    } else if (fNext < fTime) {
      FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_LIGHT);
      Point p = { ScreenWidth() / 4 + rand() % (ScreenWidth() / 2), 0 };
      ends.push_back(p);
    } else {
      FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_NIGHT);
    }

    if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

    return running;
  }
};

int main() {
  MyApp scr;
  if (scr.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) scr.Start();
  return 0;
}
