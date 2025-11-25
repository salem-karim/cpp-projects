#ifndef ENEMY_H
#define ENEMY_H

#include "../Player/Player.h"

class World;

class Enemy {
public:
  int x, y;
  int turnsToRespawn;
  void placeEnemy(Player *player);
  std::string followPlayer(World *world, Player *player);
  bool isClose(Player *player);
  void kill();
  void decreaseRespawnCounter(Player *player);
};

#endif // !ENEMY_H
