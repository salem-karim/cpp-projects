#ifndef GAME_H
#define GAME_H
// #include "Robot.hpp"
#include "World.hpp"
#include <array>

class Game {
public:
  Game();
  ~Game();
  void startGame();
  void choosePlayers();
  void gameLoop();
  void gameRound();

private:
  World *world;
  // Array<Robot, 2> players;
};

#endif // GAME_H
