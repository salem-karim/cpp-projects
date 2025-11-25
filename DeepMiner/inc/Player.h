#ifndef PLAYER_H
#define PLAYER_H

#include "Miner.h"
#include "Robot.h"

class Player : public Robot {
public:
  void move() override;
  Player(int x, int y, int score, char type, World *world);
  ~Player() { std::cout << "Player destructor called" << std::endl; }
};

#endif // !PLAYER_H
