#pragma once
#ifndef _JAEGER_HPP_
#define _JAEGER_HPP_

#include "../Schiff/Schiff.hpp"

class Jaeger : public Schiff {
public:
  Jaeger() : Schiff(75, 4, 60) {};
  void attack(ISchiff *Gegner) override;
};

#endif
