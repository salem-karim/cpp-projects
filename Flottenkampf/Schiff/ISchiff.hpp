#pragma once
#ifndef _ISCHIFF_HPP_
#define _ISCHIFF_HPP_

class Welt;

class ISchiff {
public:
  virtual ~ISchiff() {}
  virtual void attack(ISchiff *Gegner) = 0;
  virtual void move(ISchiff *Gegner, Welt *welt) = 0;
  virtual void setHuelle(int value) = 0;
  virtual void setSchaden(int value) = 0;
  virtual void setGroesse(int value) = 0;
  virtual void setIsSunk(bool value) = 0;
  virtual void setX(int value) = 0;
  virtual void setY(int value) = 0;
  virtual void setXP(int value) = 0;
  virtual int getHuelle() const = 0;
  virtual int getSchaden() const = 0;
  virtual int getGroesse() const = 0;
  virtual bool getIsSunk() const = 0;
  virtual int getX() const = 0;
  virtual int getY() const = 0;
  virtual int getXP() const = 0;
};

#endif
