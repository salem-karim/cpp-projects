#ifndef COMPUTER_H
#define COMPUTER_H

#include "Robot.h"

class Computer : public Robot {
public:
  Computer(int x, int y, int score, char type, World *world);
  void move() override;
  ~Computer() { std::cout << "Computer destructor called" << std::endl; }
};
#endif // !COMPUTER_H
