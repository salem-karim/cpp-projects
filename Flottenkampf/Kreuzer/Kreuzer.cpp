#include "Kreuzer.hpp"
#include <iostream>
#include <random>

void Kreuzer::attack(ISchiff *Gegner) {
  int xd = std::abs(getX() - Gegner->getX());
  int yd = std::abs(getY() - Gegner->getY());
  int distance = (xd + yd) - 1;
  double modifier = 1.0 / 17.0;

  // Modify the damage based on the distance, with increased range ability check
  double damageModifier =
      1.0 / (1.0 + (distance * (xp >= 20 ? modifier / 2 : modifier)));
  int modifiedDamage =
      static_cast<unsigned int>(std::round(damageModifier * getSchaden()));

  while (!Gegner->getIsSunk()) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    int random = dis(gen);

    if (random >= Gegner->getGroesse()) {
      // Apply damage reduction if the reinforced armor ability is active
      int actualDamage = modifiedDamage;
      if (xp >= 10) {
        actualDamage = static_cast<unsigned int>(modifiedDamage *
                                                 0.8); // Reduce damage by 20%
      }

      Gegner->setHuelle(Gegner->getHuelle() - actualDamage);
      std::cout << "Gegners Hülle: " << Gegner->getHuelle() << std::endl;

      // Increase XP after a successful hit
      xp++;

      if (Gegner->getHuelle() <= 0) {
        Gegner->setIsSunk(true);
        std::cout << "Destroyed" << std::endl;
      } else {
        std::cout << "Hit" << std::endl;
        continue;
      }
    } else {
      std::cout << "Missed" << std::endl;
      break;
    }
  }
}
