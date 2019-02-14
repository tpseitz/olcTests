#include "olcPixelGameEngine.h"
#include <vector>
#include <cstring>

//const int WIDTH = 80, HEIGHT = 60, PIXEL_SIZE = 1, CELL_SIZE = 10;
//const int WIDTH = 96, HEIGHT = 54, PIXEL_SIZE = 1, CELL_SIZE = 20;
const int WIDTH = 192, HEIGHT = 108, PIXEL_SIZE = 1, CELL_SIZE = 5;
//const int WIDTH = 96, HEIGHT = 54, PIXEL_SIZE = 1, CELL_SIZE = 5;
const float ROUND_LENGTH = 0.1;
const olc::Pixel COLOR_ALIVE     = olc::Pixel(192,   0,   0);
const olc::Pixel COLOR_DEAD      = olc::Pixel(  0,   0,   0);
const olc::Pixel COLOR_GRID      = olc::Pixel( 32,  32,  32);
const olc::Pixel COLOR_SELECTED  = olc::Pixel(  0,  0,  192);

struct Point { int x, y; };

class GameOfLife : public olc::PixelGameEngine {
  bool running, paused;
  float fRoundTime = 100;
  int frame = 0;
  int mouseX = -1, mouseY = -1;
  uint8_t world[WIDTH][HEIGHT] = { 0 };
  uint8_t paint = 0;
  std::vector<Point> draw;

  uint8_t Get(int x, int y) {
    if (x < 0) x += WIDTH;
    if (y < 0) y += HEIGHT;
    x %= WIDTH;
    y %= HEIGHT;
    return world[x][y];
  }

public:
  GameOfLife() {
    sAppName = "Game of life";
  }

  bool OnUserCreate() {
    running = true;
    paused = false;
    frame = -1;

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) {
    fRoundTime += fElapsedTime;
    frame++;

    if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

    if (GetKey(olc::Key::SPACE).bPressed) paused = !paused;

    bool changes = false;
    {
      int x = GetMouseX() / CELL_SIZE, y = GetMouseY() / CELL_SIZE;
      int cx = x * CELL_SIZE, cy = y * CELL_SIZE;
      DrawRect(mouseX, mouseY, CELL_SIZE, CELL_SIZE, COLOR_GRID);
      DrawRect(cx, cy, CELL_SIZE, CELL_SIZE, COLOR_SELECTED);
      mouseX = cx; mouseY = cy;
      if (GetMouse(2).bHeld) {
        world[x][y] = 0;
        changes = true;
        draw.push_back({ x, y });
      }
      if (GetMouse(0).bHeld) {
        world[x][y] = 1;
        changes = true;
        draw.push_back({ x, y });
      }
    }

    if (fRoundTime > ROUND_LENGTH && !paused) {
      uint8_t tmp[WIDTH][HEIGHT] = { 0 };

      fRoundTime = 0;
      uint8_t sum = 0, cur;
      for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
          sum = Get(x - 1, y - 1) + Get(x, y - 1) + Get(x + 1, y - 1)
            + Get(x - 1, y) + Get(x + 1, y) + Get(x - 1, y + 1)
            + Get(x, y + 1) + Get(x + 1, y + 1);
          cur = Get(x, y);
          if (cur > 0 and sum < 2) draw.push_back({ x, y });
          if (cur > 0 and (sum == 2 || sum == 3)) tmp[x][y] = 1;
          if (cur > 0 and sum > 3) draw.push_back({ x, y });
          if (cur == 0 and sum == 3) {tmp[x][y] = 1; draw.push_back({ x, y });}
        }
      }
      memcpy(&world, &tmp, WIDTH * HEIGHT);
      changes = true;
    }

    int cx = 0, cy = 0;
    for (Point &p: draw) {
      cx = p.x * CELL_SIZE;
      cy = p.y * CELL_SIZE;
      if (world[p.x][p.y] > 0)
        FillRect(cx, cy, CELL_SIZE, CELL_SIZE, COLOR_ALIVE);
      else
        FillRect(cx, cy, CELL_SIZE, CELL_SIZE, COLOR_DEAD);
      DrawRect(cx, cy, CELL_SIZE, CELL_SIZE, COLOR_GRID);
    }
    draw.clear();

    if (frame % 1000 == 0) {
      for (int x = CELL_SIZE; x < ScreenWidth(); x += CELL_SIZE)
        DrawLine(x, 0, x, ScreenHeight(), COLOR_GRID);
      for (int y = CELL_SIZE; y < ScreenHeight(); y += CELL_SIZE)
        DrawLine(0, y, ScreenWidth(), y, COLOR_GRID);
    }

    DrawRect(mouseX, mouseY, CELL_SIZE, CELL_SIZE, COLOR_SELECTED);

    return running;
  }
};

int main() {
  GameOfLife gol;
  if (gol.Construct(
    WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE,
    PIXEL_SIZE, PIXEL_SIZE)) gol.Start();
  return 0;
}

