#pragma once
#ifndef _ZERSTOERER_HPP_
#define _ZERSTOERER_HPP_

#include "../Schiff/Schiff.hpp"

class Zerstoerer : public Schiff {
public:
  Zerstoerer() : Schiff(150, 6, 60) {};
  void attack(ISchiff *Gegner) override;
};

#endif
