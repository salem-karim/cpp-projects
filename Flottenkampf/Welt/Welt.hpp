#pragma once
#ifndef _WELT_HPP_
#define _WELT_HPP_

#include <array>
#include <memory>
#include <vector>

class Schiff;
class ISchiff;

typedef std::array<std::vector<std::shared_ptr<Schiff>>, 2> Flotten;

class Welt {
public:
  Welt();
  void printWelt(Flotten Flotten);
  std::array<std::array<char, 10>, 10> getGrid() const;
  void setField(char Field, int x, int y);

private:
  std::array<std::array<char, 10>, 10> grid;
};

#endif
