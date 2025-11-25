#include "Jaeger.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

void Jaeger::attack(ISchiff *Gegner) {
  if (Gegner->getIsSunk()) {
    std::cout << "The enemy ship is already destroyed" << std::endl;
    return;
  }
  int xd = std::abs(getX() - Gegner->getX());
  int yd = std::abs(getY() - Gegner->getY());
  // numbers from 0 to 17
  int distance = (xd + yd) - 1;
  double modifier = 1.0 / 17.0;
  // Modify the damage based on the distance
  double damageModifier = 1.0 / (1.0 + (distance * modifier));
  int modifiedDamage =
      static_cast<unsigned int>(std::round(damageModifier * getSchaden()));

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 10);
  int random = dis(gen);

  // Apply damage based on the modified damage
  if (random >= Gegner->getGroesse()) {
    if (xp >= 10) {
      if (random > 6) {
        modifiedDamage *= 2;
        Gegner->setHuelle(Gegner->getHuelle() - modifiedDamage);
      } else {
        Gegner->setHuelle(Gegner->getHuelle() - modifiedDamage);
      }
    } else if (random > 8) {
      Gegner->setHuelle((Gegner->getHuelle() - modifiedDamage) * 2);
      std::cout << "Gegners Hülle: " << Gegner->getHuelle() << std::endl;
      xp++;
    } else {
      Gegner->setHuelle(Gegner->getHuelle() - modifiedDamage);
      std::cout << "Gegners Hülle: " << Gegner->getHuelle() << std::endl;
      xp++;
    }
    if (Gegner->getHuelle() <= 0) {
      Gegner->setIsSunk(true);
      std::cout << "Destroyed" << std::endl;
      return;
    } else {
      std::cout << "Hit" << std::endl;
    }
  } else {
    std::cout << "Missed" << std::endl;
  }
}
