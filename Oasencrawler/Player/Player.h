#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class World;
class Enemy;

class Player {
public:
  int x, y;
  int health, score;
  int shields, arrows;
  int isBleeding;

  Player(int x, int y);
  bool move(World *world, char direction, std::string &error);
  void shootArrow(World *world, Enemy *enemy, std::string &error);
};

#endif // !PLAYER_H
