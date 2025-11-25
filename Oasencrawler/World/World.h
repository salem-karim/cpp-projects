#ifndef WORLD_H
#define WORLD_H

#include "../Player/Player.h"

class World {
public:
  int size = 5;
  char grid[50][50];
  void initializeWorld(Player *player);
  bool checkRelics();
  void applyField(Player *player, std::string &message);
};

#endif // !WORLD_H
