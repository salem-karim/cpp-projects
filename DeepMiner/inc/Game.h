
#ifndef GAME_H
#define GAME_H

#include "Robot.h"
#include "World.h"
// #include <array>

class Game {
public:
  Game();
  ~Game();
  void gameRound();
  void gameLoop();
  void gameEnd();
  void gameCheck();
  void choosePlayers();

private:
  World *world;
  Robot *robots[2];
  // std::array<Robot*, 2> robotArray;
};

#endif // !GAME_H
