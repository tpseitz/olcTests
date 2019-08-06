#include "olcPixelGameEngine.h"

const int WIDTH = 400, HEIGHT = 300, PIXEL_SIZE = 2;
const int iMapWidth = 8, iMapHeight = 8;
const uint8_t cMap[] =
  "\x05\x05\x05\x05\x05\x05\x05\x05"
  "\x05\xff\xff\xff\xff\xff\xff\x05"
  "\x05\xff\x05\xff\xff\xff\xff\x05"
  "\x05\xff\x05\xff\xff\xff\xff\x05"
  "\x05\xff\x05\xff\xff\xff\xff\x05"
  "\x05\xff\x05\xff\xff\xff\x05\x05"
  "\x05\xff\xff\xff\xff\xff\xff\x05"
  "\x05\x05\x05\x05\x05\x05\x05\x05";

class WolfTest : public olc::PixelGameEngine {
private:
  bool bRunning = false;

public:
  WolfTest() {
    sAppName = "Wolfenstein 3D like";
  }

  bool OnUserCreate() override {
    bRunning = true;
    return true;
  }

  void drawMap() {
    for (int x = 0, y = 0; y < iMapHeight; ) {
      if (cMap[x + y * iMapHeight] < 128) Draw(x, y);
      if (++x >= iMapWidth) { x = 0; y++; }
    }
  }

  bool OnUserUpdate(float fElapsedTime) override {
    drawMap();

    if (GetKey(olc::Key::ESCAPE).bPressed) bRunning = false;

    return bRunning;
  }
};

int main(int args, char** argv) {
  WolfTest app;
  if (app.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) app.Start();
  return 0;
}
