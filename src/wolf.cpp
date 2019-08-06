#include "olcPixelGameEngine.h"

const int WIDTH = 200, HEIGHT = 150, PIXEL_SIZE = 4;
//const int WIDTH = 800, HEIGHT = 600, PIXEL_SIZE = 1;
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
  float fMaxDistance = 16.0;
  float fPlayerX = 4.0, fPlayerY = 4.0, fPlayerAngle = 2.0;
  float fPlayerSpeed = 1.0;
  float fRaySpeed = 1.0 / (float)HEIGHT * 5.0;
  float fFOV = 3.14159 / 4.5;
  olc::Pixel COLOR_WALL = 0xffffffff;
  olc::Pixel COLOR_FLOOR = 0xff800000;
  float fZBuffer[WIDTH];

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
      if (cMap[x + y * iMapHeight] < 128) Draw(x, y, olc::Pixel(0xffff0000));
      if (++x >= iMapWidth) { x = 0; y++; }
    }
    Draw((int)fPlayerX, (int)fPlayerY, olc::Pixel(0xff0000ff));
  }

  bool OnUserUpdate(float fElapsedTime) override {
    float fPlayerLeft = fPlayerAngle - 1.57079;
    float fPrevX = fPlayerX, fPrevY = fPlayerY;
    if (GetKey(olc::Key::A).bHeld) {
      fPlayerX += sin(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
      fPlayerY += cos(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::D).bHeld) {
      fPlayerX -= sin(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
      fPlayerY -= cos(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::UP).bHeld or GetKey(olc::Key::W).bHeld) {
      fPlayerX += sin(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
      fPlayerY += cos(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::DOWN).bHeld or GetKey(olc::Key::S).bHeld) {
      fPlayerX -= sin(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
      fPlayerY -= cos(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::LEFT).bHeld) {
      fPlayerAngle -= fElapsedTime;
    } if (GetKey(olc::Key::RIGHT).bHeld) {
      fPlayerAngle += fElapsedTime;
    } if (GetKey(olc::Key::ESCAPE).bPressed) bRunning = false;

/*
    if (fPlayerX < 0) fPlayerX = 0;
    if (fPlayerY < 0) fPlayerY = 0;
    if (fPlayerX >= iMapWidth)  fPlayerX = iMapWidth - 1;
    if (fPlayerY >= iMapHeight) fPlayerY = iMapHeight - 1;
*/

    if (cMap[(int)fPlayerX + (int)fPlayerY * iMapWidth] < 128) {
      fPlayerX = fPrevX; fPlayerY = fPrevY;
    }

    float fHeight = ScreenHeight() / 2, colMul;
    int maxX = ScreenWidth(), maxY = ScreenHeight();
    olc::Pixel cFloor;
    for (int y = 0; y < fHeight; y++) {
      colMul = 1.0 - (float)y / fHeight;
      cFloor = COLOR_FLOOR;
      cFloor.r *= colMul; cFloor.g *= colMul; cFloor.b *= colMul;
      DrawLine(0, y, maxX, y, cFloor);
      DrawLine(0, maxY - y, maxX, maxY - y, cFloor);
    }

    olc::Pixel cWall;
    int iMiddleY = ScreenHeight() / 2, dh;
    float ang = fPlayerAngle - fFOV / 2.0, aa = fFOV / (float)ScreenWidth();
    float px, py, sx, sy, dist;
    float fMiddleY = iMiddleY;
    for (int x = 0; x < WIDTH; x++) {
      sx = sin(ang) * fRaySpeed;
      sy = cos(ang) * fRaySpeed;
      ang += aa;
      px = fPlayerX + sx * 2.0;
      py = fPlayerY + sy * 2.0;
      for (dist = fRaySpeed; dist < fMaxDistance; dist += fRaySpeed) {
        if (px < 0 or py < 0 or px >= iMapWidth or py >= iMapHeight) break;
        if (cMap[(int)px + (int)py * iMapWidth] < 128) break;
        px += sx; py += sy;
      }
      fZBuffer[x] = dist;
      dh = (1.5 / dist) * fMiddleY;
      if (dh > fMiddleY) dh = fMiddleY;
      colMul = 1.0 - dist / fMaxDistance;
      cWall = COLOR_WALL;
      cWall.r *= colMul; cWall.g *= colMul; cWall.b *= colMul;
      DrawLine(x, iMiddleY - dh, x, iMiddleY + dh, cWall);
    }

    drawMap();

    return bRunning;
  }
};

int main(int args, char** argv) {
  WolfTest app;
  if (app.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) app.Start();
  return 0;
}
