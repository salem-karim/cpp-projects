#ifndef ROBOT_H
#define ROBOT_H

#include "Miner.h"
#include "World.h"

class Robot : public IMiner {
public:
  void mine() override;
  void moveToSmallest() override;
  void removeLastElement(int x, int y) override;

  int getX() const override;
  int getY() const override;
  int getScore() const override;
  World getWorld() const override;
  char getType() const override;

  void setX(int x) override;
  void setY(int y) override;
  void setType(char type) override;
  void setScore(int score) override;
  void setWorld(World *world) override;

  // Robot(int x, int y, int score, Type type, World *world);
  virtual ~Robot();

protected:
  int x;
  int y;
  int score;
  char type;
  World *world;
};
#endif // !ROBOT_H
