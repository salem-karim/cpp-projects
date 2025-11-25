#include "Schiff.hpp"
#include "../Welt/Welt.hpp"
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>

int Schiff::getHuelle() const { return Huelle; }
int Schiff::getSchaden() const { return Schaden; }
int Schiff::getGroesse() const { return Groesse; }
bool Schiff::getIsSunk() const { return isSunk; }
int Schiff::getX() const { return x; }
int Schiff::getY() const { return y; }
int Schiff::getXP() const { return xp; }
void Schiff::setHuelle(int value) { Huelle = value; }
void Schiff::setSchaden(int value) { Schaden = value; }
void Schiff::setGroesse(int value) { Groesse = value; }
void Schiff::setIsSunk(bool value) { isSunk = value; }
void Schiff::setX(int value) { x = value; }
void Schiff::setY(int value) { y = value; }
void Schiff::setXP(int value) { xp = value; }
Schiff::Schiff(int shell, int size, int damage) {
  Huelle = shell;
  Groesse = size;
  Schaden = damage;
}

void Schiff::move(ISchiff *Gegner, Welt *welt) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 1);

  // Get the grid
  const auto &grid = welt->getGrid();

  // Get the x distance and y distance to the target
  int xDist = std::abs(Gegner->getX() - x);
  int yDist = std::abs(Gegner->getY() - y);

  // If x distance is greater than y distance, move in x direction
  if (xDist > yDist) {
    try {
      if (Gegner->getX() > x) {
        if (grid.at(x + 1).at(y) == '-') {
          x++;
        }
      } else {
        if (grid.at(x - 1).at(y) == '-') {
          x--;
        }
      }
    } catch (const std::out_of_range &e) {
      // Handle out-of-bounds exception
      std::cerr << "Out of bounds: " << e.what() << std::endl;
    }
  } else if (xDist < yDist) {
    try {
      if (Gegner->getY() > y) {
        if (grid.at(x).at(y + 1) == '-') {
          y++;
        }
      } else {
        if (grid.at(x).at(y - 1) == '-') {
          y--;
        }
      }
    } catch (const std::out_of_range &e) {
      // Handle out-of-bounds exception
      std::cerr << "Out of bounds: " << e.what() << std::endl;
    }
  } else {
    // Randomly choose x or y direction
    if (dis(gen) == 0) {
      try {
        if (Gegner->getX() > x) {
          if (grid.at(x + 1).at(y) == '-') {
            x++;
          }
        } else {
          if (grid.at(x - 1).at(y) == '-') {
            x--;
          }
        }
      } catch (const std::out_of_range &e) {
        // Handle out-of-bounds exception
        std::cerr << "Out of bounds: " << e.what() << std::endl;
      }
    } else {
      try {
        if (Gegner->getY() > y) {
          if (grid.at(x).at(y + 1) == '-') {
            y++;
          }
        } else {
          if (grid.at(x).at(y - 1) == '-') {
            y--;
          }
        }
      } catch (const std::out_of_range &e) {
        // Handle out-of-bounds exception
        std::cerr << "Out of bounds: " << e.what() << std::endl;
      }
    }
  }
}
