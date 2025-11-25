#include "Welt.hpp"
#include "../Jaeger/Jaeger.hpp"
#include "../Kreuzer/Kreuzer.hpp"
#include "../Zerstoerer/Zerstoerer.hpp"
#include <array>
#include <iostream>

using namespace std;

Welt::Welt() {
  for (auto &row : grid)
    for (auto &field : row)
      field = '-';
}
std::array<std::array<char, 10>, 10> Welt::getGrid() const { return grid; }
void Welt::setField(char Field, int x, int y) { grid.at(x).at(y) = Field; }

void Welt::printWelt(Flotten flotten) {
  char symbol = ' ';

  for (auto &row : grid)
    for (auto &field : row)
      field = '-';

  // Mark ships on the grid
  for (int playerIndex = 0; playerIndex < flotten.size(); ++playerIndex) {
    const auto &fleet = flotten.at(playerIndex);
    for (const auto &schiff : fleet) {
      int x = schiff->getX();
      int y = schiff->getY();
      if (x >= 0 && x < grid.size() && y >= 0 && y < grid.at(0).size() &&
          !schiff->getIsSunk()) {
        if (dynamic_cast<Jaeger *>(schiff.get())) {
          symbol = (playerIndex == 0) ? 'J' : 'j';
        } else if (dynamic_cast<Zerstoerer *>(schiff.get())) {
          symbol = (playerIndex == 0) ? 'Z' : 'z';
        } else if (dynamic_cast<Kreuzer *>(schiff.get())) {
          symbol = (playerIndex == 0) ? 'K' : 'k';
        } else {
          symbol = 'X'; // Default marking for unknown type
        }
        grid.at(x).at(y) = symbol;
      }
    }
  }

  // Print the grid
  for (const auto &row : grid) {
    for (const auto &field : row) {
      cout << field << ' ';
    }
    cout << endl;
  }
}
