#pragma once
#include <memory>
#ifndef _SPIEL_HPP_
#define _SPIEL_HPP_

#include "../Schiff/Schiff.hpp"
#include "../Welt/Welt.hpp"
#include <array>
#include <vector>

class Spiel {
public:
  Spiel();
  void spielStart();
  void spielLoop();
  void spielRunde();
  void spielEnde();
  void flotteErstellen();
  void flottenInitialisieren();

private:
  std::array<std::vector<std::shared_ptr<Schiff>>, 2> Flotten;
  std::shared_ptr<Welt> spielFeld;
};

#endif
