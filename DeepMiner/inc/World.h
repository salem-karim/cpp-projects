#ifndef WORLD_H
#define WORLD_H

#include <vector>
using layers = std::vector<std::vector<int>>;

class World {
public:
  World(int depth);
  ~World();
  void shuffleWorld();
  void rerollWorld();
  void printWorld(int x1, int y1, int x2, int y2);
  void printTopLayer(int x1, int y1, int x2, int y2);
  void sortWorld(bool ascending);
  void sortPillar(int x, int y, bool ascending);
  int getLastElement(int x, int y) const;
  std::vector<layers> getGrid() const;

private:
  std::vector<layers> Grid;
};

#endif // !WORLD_H
