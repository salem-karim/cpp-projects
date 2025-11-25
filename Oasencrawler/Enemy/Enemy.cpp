#include "Enemy.h"
#include "../World/World.h"
#include <cstdlib>

std::string Enemy::followPlayer(World *world, Player *player) {
  int dx = player->x - x, dy = player->y - y;

  if (std::abs(dx) == std::abs(dy)) {
    // If diagonally aligned, move randomly in one dimension
    if (rand() % 2 == 0) { // Randomly choose between x and y dimensions
      if (dx > 0 && x < world->size - 1) {
        x++; // Move right if possible
      } else if (dx < 0 && x > 0) {
        x--; // Move left if possible
      }
    } else {
      if (dy > 0 && y < world->size - 1) {
        y++; // Move down if possible
      } else if (dy < 0 && y > 0) {
        y--; // Move up if possible
      }
    }
  } else {
    // Move towards the player in the dimension with the greater difference
    if (std::abs(dx) > std::abs(dy)) {
      if (dx > 0 && x < world->size - 1) {
        x++; // Move right if possible
      } else if (dx < 0 && x > 0) {
        x--; // Move left if possible
      }
    } else {
      if (dy > 0 && y < world->size - 1) {
        y++; // Move down if possible
      } else if (dy < 0 && y > 0) {
        y--; // Move up if possible
      }
    }
    if (x == player->x && y == player->y) {
      player->health -= 1;
      placeEnemy(player);
      return "You were attacked by an enemy!";
    }
  }
  return " ";
}

// check if Enemy is in sorunding of player

bool Enemy::isClose(Player *player) {
  // Get the distance of the player to the enemy
  int dx = std::abs(x - player->x);
  int dy = std::abs(y - player->y);
  return (dx <= 1 && dy <= 1);
}

void Enemy::placeEnemy(Player *player) {
  turnsToRespawn = 0; // Reset the respawn counter
  do {
    x = rand() % 5;
    y = rand() % 5;
  } while (isClose(player));
}

void Enemy::kill() {
  turnsToRespawn = 4; // Set the respawn counter to 3 turns
  x = -1;             // Move the enemy out of the grid temporarily
  y = -1;
}

// Implementation of decreaseRespawnCounter method
void Enemy::decreaseRespawnCounter(Player *player) {
  if (turnsToRespawn > 0) {
    turnsToRespawn--;
    if (turnsToRespawn == 0) {
      // Respawn the enemy at a new location
      placeEnemy(player);
    }
  }
}
