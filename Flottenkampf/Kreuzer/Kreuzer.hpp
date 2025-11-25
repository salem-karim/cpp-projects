#pragma once
#ifndef _KREUZER_HPP_
#define _KREUZER_HPP_

#include "../Schiff/Schiff.hpp"

class Kreuzer : public Schiff {
public:
  Kreuzer() : Schiff(250, 8, 50) {};
  void attack(ISchiff *Gegner) override;
};
#endif
