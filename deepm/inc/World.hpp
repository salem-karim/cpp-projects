#ifndef WORLD_H
#define WORLD_H
#include <array>
#include <deque>

using layers = std::array<std::array<int, 5>, 5>;

class World {
public:
  World(int depth);
  void initWorld();

  void setLength(int length);
  void setWidth(int width);
  void setDepth(int depth);

  int getLength();
  int getWidth();
  int getDepth();

private:
  std::deque<layers> Grid;
  int length;
  int width;
  int depth;
};

#endif // WORLD_H
