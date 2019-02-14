#include <vector>
#include <map>
#include <iostream>
#include "vector.h"
#include "network.h"

Node::Node() {
  id = 0;
  location = Vector(-1, -1);
}

Node::Node(Vector &location) {
  id = ++node_index;
  this->location = location;
}

void Node::AddPathSingle(Node* next) {
  paths.push_back(next);
}

void Node::AddPathDuplex(Node* next) {
  paths.push_back(next);
  next->AddPathSingle(this);
}

uint32_t Node::GetID() { return id; }

Vector Node::GetLocation() { return location; }

std::vector<Node*> Node::GetNeighbours() { return paths; }

Network::~Network() {
  for (std::pair<int, Node*> p: nodes) delete p.second;
}

Node* Network::AddNode(Vector location, uint32_t link) {
  Node *nd = new Node(location);
  nodes[nd->GetID()] = nd;
  if (link != 0) AddPath(nd->GetID(), link);
  return nd;
}

void Network::Simplify() {
  Node* nd;
  for (std::pair<int, Node*> p: nodes) {
    if (p.second == NULL) continue;
    nd = p.second;
    if (nd->paths.size() == 2) nd->type = Path;
    else if (nd->paths.size() > 2) nd->type = Crossroad;
    else if (nd->paths.size() == 1) nd->type = End;
    else nd->type = Room;

    if (nd->type == Path) {
      Node *n1 = nd->paths[0], *n2 = nd->paths[1];
      if ((n1->location - nd->location).dot(nd->location - n2->location) < 0.9) nd->type = Corner;
    }
  }
}

void Network::AddPath(uint32_t start, uint32_t end) {
  Node *n1 = nodes[start], *n2 = nodes[end];
  if (n1 == NULL or n2 == NULL) {
    std::cerr << "Illegal node id" << std::endl;
    return;
  }
  n1->AddPathDuplex(n2);
}

std::vector<Node*> Network::ListNodes() {
  std::vector<Node*> ls;
  for (std::pair<int, Node*> p: nodes) ls.push_back(p.second);
  return ls;
}
