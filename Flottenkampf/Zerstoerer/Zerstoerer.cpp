#include "Zerstoerer.hpp"
#include <iostream>
#include <random>

void Zerstoerer::attack(ISchiff *Gegner) {
  int loop = 1;
  if (xp >= 5)
    int loop = 2;
  for (int i = 0; i < loop; i++) {
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
    if (random >= (Gegner->getGroesse() - 2)) {
      Gegner->setHuelle(Gegner->getHuelle() - modifiedDamage);
      xp++;
      std::cout << "Gegners Hülle: " << Gegner->getHuelle() << std::endl;
      if (Gegner->getHuelle() <= 0) {
        Gegner->setIsSunk(true);
        std::cout << "Destroyed" << std::endl;
        return;
      } else {
        std::cout << "Hit" << std::endl;
        // 50% cahnce to get 30 health back when xp >= 10
        if (xp >= 10) {
          std::uniform_int_distribution<> disHealth(1, 2);
          int healthChance = disHealth(gen);
          if (healthChance == 1) {
            setHuelle(getHuelle() + 30);
            std::cout << "Zerstörer regained 30 health!" << std::endl;
          }
        }
      }
    } else {
      std::cout << "Missed" << std::endl;
    }
  }
}
