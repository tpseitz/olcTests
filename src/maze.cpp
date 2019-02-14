#include <vector>
#include "olcPixelGameEngine.h"
#include "vector.h"
#include "network.h"

//const int WIDTH = 480, HEIGHT = 270, PIXEL_SIZE = 1;
//const int GRID_WIDTH = 48, GRID_HEIGHT = 27;
const int WIDTH = 960, HEIGHT = 540, PIXEL_SIZE = 1;
const int GRID_WIDTH = 96, GRID_HEIGHT = 54;
//const int WIDTH = 683, HEIGHT = 384, PIXEL_SIZE = 1;
//const int GRID_WIDTH = 68, GRID_HEIGHT = 38;
olc::Pixel COLOR_BACKGROUND = olc::Pixel(  0,   0,   0);
olc::Pixel COLOR_NETWORK    = olc::Pixel(  0,   0, 192);
olc::Pixel COLOR_INACTIVE   = olc::Pixel( 32,  32,  32);
olc::Pixel COLOR_PATH       = olc::Pixel(128, 128, 128);
olc::Pixel COLOR_THREAD     = olc::Pixel(192,   0,   0);

Node empty;

class MazeApp : public olc::PixelGameEngine {
private:
  bool running = false, done = false;
  Node* grid[GRID_WIDTH][GRID_HEIGHT] = { NULL };
  Network maze;
  Node *head;
  std::vector<Node*> tail;

  Node* GetCell(int x, int y) {
    if (x < 0 or y < 0 or x >= GRID_WIDTH or y >= GRID_HEIGHT) return &empty;
    else return grid[x][y];
  }

public:
  MazeApp() {
    Vector hv = { (double)(rand() % GRID_WIDTH),
      (double)(rand() % GRID_HEIGHT) };
    head = maze.AddNode(hv);
    grid[(int)hv.x][(int)hv.y] = head;
  }

  bool OnUserCreate() override {
    running = true;

    return running;
  }

  void Cycle() {
    int x = (int)head->GetLocation().x, y = (int)head->GetLocation().y;
    std::vector<Vector> ls;
    if (GetCell(x, y - 1) == NULL) ls.push_back(Vector(x, y - 1));
    if (GetCell(x - 1, y) == NULL) ls.push_back(Vector(x - 1, y));
    if (GetCell(x, y + 1) == NULL) ls.push_back(Vector(x, y + 1));
    if (GetCell(x + 1, y) == NULL) ls.push_back(Vector(x + 1, y));

    if (ls.empty()) {
      if (tail.empty()) {
        done = true;
        return;
      } else {
        head = tail.back();
        tail.pop_back();
      }
    } else {
      tail.push_back(head);
      Vector hv = { ls[rand() % ls.size()] };
      head = maze.AddNode(hv, tail.back()->GetID());
      grid[(int)hv.x][(int)hv.y] = head;
    }
  }

  void DrawMaze() {
    Vector hv = head->GetLocation(), gv, v;
    for (int x = -1; x < 2; x++) {
      for (int y = -1; y < 2; y++) {
        gv = hv + Vector(x, y);
        Node* n1 = GetCell((int)gv.x, (int)gv.y);
        if (n1 == &empty) continue;
        if (n1 != NULL and n1->location.x >= 0) {
          Vector v1 = n1->GetLocation() * 10 + Vector(5, 5);
          for (Node* n2: n1->GetNeighbours()) {
            Vector v2 = n2->GetLocation() * 10 + Vector(5, 5);
            int x = (int)v1.x, y = (int)v1.y, w = 9, h = 9;
            if (v1.x == v2.x) h = 19;
            else w = 19;
            if (x > v2.x) x = (int)v2.x;
            if (y > v2.y) y = (int)v2.y;
            x -= 4; y -= 4;
            FillRect(x, y, w, h, COLOR_PATH);
          }
        } else {
          v = gv * 10;
          FillRect((int)v.x, (int)v.y, 10, 10, COLOR_BACKGROUND);
        }
      }
    }

    FillRect((int)hv.x * 10 + 3, (int)hv.y * 10 + 3, 5, 5, COLOR_THREAD);
  }

  void DrawMazeFull() {
    FillRect(0, 0, ScreenWidth(), ScreenHeight(), COLOR_BACKGROUND);

    for (Node* n1: maze.ListNodes()) {
      Vector v1 = n1->GetLocation() * 10 + Vector(5, 5);
      for (Node* n2: n1->GetNeighbours()) {
        Vector v2 = n2->GetLocation() * 10 + Vector(5, 5);
        int x = (int)v1.x, y = (int)v1.y, w = 9, h = 9;
        if (v1.x == v2.x) h = 19;
        else w = 19;
        if (x > v2.x) x = (int)v2.x;
        if (y > v2.y) y = (int)v2.y;
        x -= 4; y -= 4;
        FillRect(x, y, w, h, COLOR_PATH);
      }
    }
  }

  void DrawNetwork() {
    for (Node* n1: maze.ListNodes()) {
      Vector v1 = n1->GetLocation() * 10 + Vector(5, 5);
      for (Node* n2: n1->GetNeighbours()) {
        Vector v2 = n2->GetLocation() * 10 + Vector(5, 5);
        DrawLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, COLOR_NETWORK);
      }
      if (n1->type == End)
        FillRect((int)v1.x - 2, (int)v1.y - 2, 5, 5, COLOR_NETWORK);
      else if (n1->type == Crossroad)
        FillCircle((int)v1.x, (int)v1.y, 2, COLOR_NETWORK);
      else if (n1->type == Corner) {
        FillRect((int)v1.x - 1, (int)v1.y - 1, 3, 3, COLOR_NETWORK);
/*
        DrawLine((int)v1.x - 1, (int)v1.y,
          (int)v1.x + 1, (int)v1.y, COLOR_NETWORK);
        DrawLine((int)v1.x, (int)v1.y - 1,
          (int)v1.x, (int)v1.y + 1, COLOR_NETWORK);
*/
      }
    }
  }

  void DrawThread() {
    if (tail.empty()) return;

    Vector v1 = head->GetLocation(), v2;
    for (int i = tail.size() - 1; i >= 0; i--) {
      v2 = tail[i]->GetLocation();
      DrawLine((int)v1.x * 10 + 5, (int)v1.y * 10 + 5,
        (int)v2.x * 10 + 5, (int)v2.y * 10 + 5, COLOR_THREAD);
      v1 = v2;
    }

    Vector hv = head->GetLocation();
    FillRect((int)hv.x * 10 + 3, (int)hv.y * 10 + 3, 5, 5, COLOR_THREAD);
  }

  bool OnUserUpdate(float fElapsedTime) override {
    if (GetKey(olc::Key::ESCAPE).bPressed) running = false;

//    while (!done) Cycle();
    if (done) return running;
    Cycle();
    DrawMaze();
    maze.Simplify();
    DrawNetwork();
    DrawThread();

    return running;
  }
};

int main() {
  MazeApp maze;
  if (maze.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) maze.Start();
  return 0;
}

