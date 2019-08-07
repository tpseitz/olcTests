#include "olcPixelGameEngine.h"

const int WIDTH = 200, HEIGHT = 150, PIXEL_SIZE = 4;
//const int WIDTH = 400, HEIGHT = 300, PIXEL_SIZE = 2;
//const int WIDTH = 800, HEIGHT = 600, PIXEL_SIZE = 1;
const int WALL_TEXTURES = 128;
const int iMapWidth = 16, iMapHeight = 16;
uint8_t cMap[] =
  "HHHHHHHHHHHHHHHH"
  "H....I.........H"
  "H....I.........H"
  "H....IIIII.....H"
  "H....I.........H"
  "HI.III.........H"
  "H..............H"
  "H..............H"
  "H.....III.III..H"
  "H.....I.....I..H"
  "H.....I.....I..H"
  "H.....I.....I..H"
  "H.....III.III..H"
  "H..............H"
  "H..............H"
  "HHHHHHHHHHHHHHHH";

class WolfTest : public olc::PixelGameEngine {
private:
  bool bRunning = false;
  float fMaxDistance = 24.0;
  float fPlayerX = 4.0, fPlayerY = 4.0, fPlayerAngle = 3.141592653589793;
  float fPlayerSpeed = 1.0;
  float fRaySpeed = 1.0 / (float)HEIGHT * 5.0;
  float fFOV = 0.6981317007977318;
  olc::Pixel COLOR_WALL = 0xffffffff;
  olc::Pixel COLOR_FLOOR = 0xff7f0000;
  olc::Sprite *tWall[WALL_TEXTURES] = { NULL };
  float fZBuffer[WIDTH];

public:
  WolfTest() {
    sAppName = "Wolfenstein 3D like";
  }

  bool OnUserCreate() override {
    for (int i = 0; i < iMapWidth * iMapHeight; i++)
      if (cMap[i] == '.') cMap[i] = 192;

    tWall[0] = NULL;
    tWall[1] = new olc::Sprite("gfx/wall_01.png");

    bRunning = true;
    return true;
  }

  bool OnUserDestroy() {
    for (olc::Sprite* pt: tWall) delete pt;

    return true;
  }

  void drawMap() {
    for (int x = 0, y = 0; y < iMapHeight; ) {
      if (cMap[x + y * iMapHeight] < WALL_TEXTURES)
        Draw(x, y, olc::Pixel(0xffff0000));
      if (++x >= iMapWidth) { x = 0; y++; }
    }
    Draw((int)fPlayerX, (int)fPlayerY, olc::Pixel(0xff0000ff));
  }

  bool OnUserUpdate(float fElapsedTime) override {
    float fPlayerLeft = fPlayerAngle - 1.57079;
    float fPrevX = fPlayerX, fPrevY = fPlayerY;
    if (GetKey(olc::Key::A).bHeld) {
      fPlayerX += -sin(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
      fPlayerY += cos(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::D).bHeld) {
      fPlayerX -= -sin(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
      fPlayerY -= cos(fPlayerLeft) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::UP).bHeld or GetKey(olc::Key::W).bHeld) {
      fPlayerX += -sin(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
      fPlayerY += cos(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
    } if (GetKey(olc::Key::DOWN).bHeld or GetKey(olc::Key::S).bHeld) {
      fPlayerX -= -sin(fPlayerAngle) * fElapsedTime * fPlayerSpeed;
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

    if (cMap[(int)fPlayerX + (int)fPlayerY * iMapWidth] < WALL_TEXTURES) {
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
    int iMiddleY = ScreenHeight() / 2, dh, bi;
    uint16_t block;
    float ang = fPlayerAngle - fFOV / 2.0, aa = fFOV / (float)ScreenWidth();
    float px, py, sx, sy, dist;
    double tmp;
    float fMiddleY = iMiddleY;
    for (int x = 0; x < WIDTH; x++) {
      sx = -sin(ang) * fRaySpeed;
      sy = cos(ang) * fRaySpeed;
      ang += aa;
      px = fPlayerX + sx * 2.0;
      py = fPlayerY + sy * 2.0;
      block = 0;
      for (dist = fRaySpeed; dist < fMaxDistance; dist += fRaySpeed) {
        if (px < 0 or py < 0 or px >= iMapWidth or py >= iMapHeight) break;
        bi = (int)px + (int)py * iMapWidth;
        if (cMap[bi] < WALL_TEXTURES) { block = cMap[bi]; break; }
        px += sx; py += sy;
      }
      fZBuffer[x] = dist;
      dh = (1.5 / dist) * fMiddleY;
      float v = 0.0, inc = 1.0 / (float)(dh * 2);
      float ux = modf(px, &tmp), uy = modf(py, &tmp), u;
      if (std::abs(ux - 0.5) > std::abs(uy - 0.5)) u = uy;
      else u = ux;
      if (dh > fMiddleY) { v += inc * (float)(dh - fMiddleY); dh = fMiddleY; }
      colMul = 1.0 - dist / fMaxDistance;
      block = 1;
      if (tWall[block] != NULL) {
        for (int y = iMiddleY - dh; y <= iMiddleY + dh; y++) {
          cWall = tWall[block]->Sample(u, v);
          v += inc;
          cWall.r *= colMul; cWall.g *= colMul; cWall.b *= colMul;
          DrawLine(x, y, x, y, cWall);
        }
      } else {
        cWall = COLOR_WALL;
        cWall.r *= colMul; cWall.g *= colMul; cWall.b *= colMul;
        DrawLine(x, iMiddleY - dh, x, iMiddleY + dh, cWall);
      }
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
