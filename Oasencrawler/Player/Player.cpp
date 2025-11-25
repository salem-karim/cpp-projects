#include "Player.h"
#include "../Enemy/Enemy.h"
#include "../World/World.h"
#include <iostream>

Player::Player(int x, int y) : x(x), y(y) {
  arrows = 0;
  isBleeding = 0;
  shields = 1;
  health = 5;
  score = 0;
}

bool Player::move(World *world, char direction, std::string &error) {
  int dx = 0, dy = 0;
  switch (direction) {
  case 'W':
  case 'w':
    dy = -1;
    break;
  case 'A':
  case 'a':
    dx = -1;
    break;
  case 'S':
  case 's':
    dy = 1;
    break;
  case 'D':
  case 'd':
    dx = 1;
    break;
  case 'x':
    break;
  default:
    error = "Invalid input!";
    return true;
  }

  if ((0 <= (this->x + dx) && (this->x + dx) < world->size) &&
      (0 <= (this->y + dy) && (this->y + dy) < world->size)) {
    this->x += dx;
    this->y += dy;
  } else {
    error = "You can't move there!";
    return true;
  }
  return false;
}

void Player::shootArrow(World *world, Enemy *enemy, std::string &error) {
  if (arrows == 0) {
    error = "You don't have any arrows!";
    return;
  }

  char input = ' ';
  std::cout << "Where do you want to shoot? (w, a, s, d): ";
  std::cin >> input;

  int dx = 0, dy = 0;
  switch (input) {
  case 'W':
  case 'w':
    dy = -1;
    break;
  case 'S':
  case 's':
    dy = 1;
    break;
  case 'A':
  case 'a':
    dx = -1;
    break;
  case 'D':
  case 'd':
    dx = 1;
    break;
  default:
    error = "Invalid input!";
    return;
  }

  // Check the entire axis in the specified direction
  int newX = x;
  int newY = y;
  while (newX >= 0 && newX < world->size && newY >= 0 && newY < world->size) {
    // Check if the position contains an enemy
    if (newX == enemy->x && newY == enemy->y) {
      enemy->kill();
      arrows--;

      error = "You hit an Enemy!";
      return;
    }

    // Move to the next position along the axis
    newX += dx;
    newY += dy;
  }

  // If no enemy is found along the entire axis
  error = "There is no enemy in this direction!";
}
