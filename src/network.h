#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <map>
#include <iostream>
#include "vector.h"

static uint32_t node_index = 0;

enum NodeType { Room, End, Corner, Path, Crossroad };

struct Node {
  uint32_t id = 0;
  NodeType type;
  Vector location;
  std::vector<Node*> paths;

  Node();
  Node(Vector&);
  void AddPathSingle(Node*);
  void AddPathDuplex(Node*);
  uint32_t GetID();
  Vector GetLocation();
  std::vector<Node*> GetNeighbours();
};

struct Network {
  std::map<int, Node*> nodes;

  ~Network();
  Node* AddNode(Vector, uint32_t=0);
  void Simplify();
  void AddPath(uint32_t, uint32_t);
  std::vector<Node*> ListNodes();
};

#endif
