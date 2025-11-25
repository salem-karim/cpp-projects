#ifndef MINER_H
#define MINER_H

#include "World.h"
#include <iostream>

class IMiner {
public:
  virtual void move() = 0;
  virtual void mine() = 0;
  virtual void moveToSmallest() = 0;
  virtual void removeLastElement(int x, int y) = 0;

  virtual int getX() const = 0;
  virtual int getY() const = 0;
  virtual int getScore() const = 0;
  virtual World getWorld() const = 0;
  virtual char getType() const = 0;

  virtual void setX(int x) = 0;
  virtual void setY(int y) = 0;
  virtual void setScore(int score) = 0;
  virtual void setType(char type) = 0;
  virtual void setWorld(World *world) = 0;
  virtual ~IMiner() { std::cout << "IMiner destructor called" << std::endl; }
};

#endif // !MINER_H
