#include "World.h"
#include <cstdlib>

void World::initializeWorld(Player *player) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      int randomnum = rand() % 10;
      if (randomnum < 4) {
        grid[i][j] = '-';
      } else if (randomnum < 8) {
        grid[i][j] = 'x';
        int damagestep = rand() % 6;
        if (damagestep == 0) {
        }
      } else if (randomnum == 8) {
        grid[i][j] = '+';
      } else if (randomnum == 9) {
        grid[i][j] = '*';
      }
    }
  }
  grid[player->x][player->y] = '-';
}

bool World::checkRelics() {
  bool Relics = false;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (grid[i][j] == '*') {
        Relics = true;
      }
    }
  }
  return Relics;
}

void World::applyField(Player *player, std::string &message) {
  int randomnum = rand() % 6;
  if (player->isBleeding > 0 && player->isBleeding != 0) {
    player->isBleeding -= 1;
    player->health -= 1;
    message = "You are bleeding!";
  } else if (grid[player->x][player->y] == 'x') {
    grid[player->x][player->y] = '-';
    if (player->shields == 0 && randomnum > 3) {
      if (player->isBleeding == 0) {
        player->isBleeding = 3;
        player->health -= 1;
        message = "You are bleeding!";
      }
    } else if (player->shields > 0) {
      player->shields -= 1;
      message = "You have been protected!";
    }
  }
  if (grid[player->x][player->y] == '+') {
    grid[player->x][player->y] = '-';
    player->health += 1;
    player->isBleeding = 0;
    message = "You got 1 Healthpoint back!";
    if (randomnum >= 0 && randomnum < 2) {
      player->arrows += 1;
      message += " | You got 1 Arrow!";
    } else if (randomnum > 1 && randomnum < 4) {
      player->shields += 1;
      message += " | You got 1 Shield!";
    }
  } else if (grid[player->x][player->y] == '*') {
    grid[player->x][player->y] = '-';
    player->score += 1;
    message =
        "You found a Relic!\nyour score is: " + std::to_string(player->score);
  }
}
