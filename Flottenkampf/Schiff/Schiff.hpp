#pragma once
#ifndef _SCHIFF_HPP_
#define _SCHIFF_HPP_

#include "ISchiff.hpp"

class Welt;

class Schiff : public ISchiff {
public:
  virtual ~Schiff() {}
  Schiff(int shell, int size, int damage);
  void move(ISchiff *Gegner, Welt *welt) override;
  void setHuelle(int value) override;
  void setSchaden(int value) override;
  void setGroesse(int value) override;
  void setIsSunk(bool value) override;
  void setX(int value) override;
  void setY(int value) override;
  void setXP(int value) override;
  int getHuelle() const override;
  int getSchaden() const override;
  int getGroesse() const override;
  bool getIsSunk() const override;
  int getX() const override;
  int getY() const override;
  int getXP() const override;

protected:
  int Huelle;
  int Schaden;
  int Groesse;
  bool isSunk = false;
  int x;
  int y;
  int xp;
};

#endif
