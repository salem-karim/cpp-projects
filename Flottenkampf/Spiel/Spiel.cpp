#include "Spiel.hpp"
#include "../Jaeger/Jaeger.hpp"
#include "../Kreuzer/Kreuzer.hpp"
#include "../Zerstoerer/Zerstoerer.hpp"
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#define CLEAR "cls"
#else // In any other OS
#define CLEAR "clear"
#endif

using namespace std;

void Spiel::spielStart() {
  flotteErstellen();
  spielLoop();
}

void Spiel::spielLoop() {
  bool gameOver = false;
  char input;
  do {
    // Check if any player's fleet is completely sunk
    for (int i = 0; i < 2; i++) {
      gameOver = true;
      for (const auto &schiff : Flotten.at(i)) {
        if (!schiff->getIsSunk()) {
          gameOver = false;
          break;
        }
      }
      if (gameOver)
        break;
    }
    if (gameOver)
      break;
    spielFeld->printWelt(Flotten);
    cout << "Runde Spielen? (y/n)" << endl;
    try {
      cin >> input;
      if (cin.fail()) { // Check if the input is not a character
        cin.clear();    // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Eingabe muss ein Buchstabe sein.");
      }
      if (input != 'y' && input != 'n') {
        cout << "Bitte 'y' oder 'n' eingeben" << endl;
      }
      if (input == 'y') {
        spielRunde();
      } else if (input == 'n') {
        break; // Exit the loop if player chooses 'n'
      }
    } catch (const invalid_argument &e) {
      cout << e.what() << endl;
    }
    // system(CLEAR);
  } while (input != 'n');
}
void Spiel::spielRunde() {
  // Change which Player Shoots and which ship to attack by random device
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(0, 1);
  int player = dis(gen);

  uniform_int_distribution<> dis2(0, Flotten.at(player).size() - 1);
  int ship = dis2(gen);

  int target;
  bool targetFound = false;

  // Keep generating new targets until a non-sunk ship is found
  while (!targetFound) {
    uniform_int_distribution<> dis3(0, Flotten.at(1 - player).size() - 1);
    target = dis3(gen);

    // Check if the target ship is not sunk
    if (!Flotten.at(1 - player).at(target)->getIsSunk()) {
      targetFound = true;
    }
  }
  const auto &attacker = Flotten.at(player).at(ship);
  const auto &Ziel = Flotten.at(1 - player).at(target);

  // cout which player attacks with which ship for both players
  string position1 = '[' + to_string(attacker->getX()) + ',' +
                     to_string(attacker->getY()) + ']';
  string position2 =
      '[' + to_string(Ziel->getX()) + ',' + to_string(Ziel->getY()) + ']';
  cout << "Spieler " << player + 1 << " greift mit Schiff bei " << position1
       << " XP: " << attacker->getXP() << " Spieler " << 1 - player + 1
       << "` GegnerSchiff bei " << position2 << " an. XP: " << Ziel->getXP()
       << endl;
  // Attack the non-sunk target ship
  attacker->attack(Ziel.get());
  attacker->move(Ziel.get(), spielFeld.get());

  if (Jaeger *jaeger = dynamic_cast<Jaeger *>(attacker.get())) {
    if (jaeger->getXP() >= 5) { // Example XP threshold for additional move
      jaeger->move(Ziel.get(), spielFeld.get());
    }
  }
}

void Spiel::spielEnde() {
  spielFeld->printWelt(Flotten);
  int sunkenShips[2] = {0, 0};
  for (int i = 0; i < 2; i++) {
    for (const auto &schiff : Flotten.at(i)) {
      if (schiff->getIsSunk()) {
        sunkenShips[i]++;
      }
    }
  }
  cout << "Spieler 1 hat " << sunkenShips[1] << " Schiffe versenkt." << endl;
  cout << "Spieler 2 hat " << sunkenShips[0] << " Schiffe versenkt." << endl;
  if (sunkenShips[1] > sunkenShips[0]) {
    cout << "Spieler 1 hat gewonnen!" << endl;
  } else if (sunkenShips[1] < sunkenShips[0]) {
    cout << "Spieler 2 hat gewonnen!" << endl;
  } else {
    cout << "Unentschieden!" << endl;
  }
}

void Spiel::flotteErstellen() {
  int length = 0;
  int type = 0;

  for (int i = 0; i < 2; i++) {
    do {
      cout << "Größe der Flotte(1-9) von Spieler " << i + 1 << ": ";
      try {
        cin >> length;
        if (cin.fail()) { // Check if the input is not an integer
          cin.clear();    // Clear the error flag
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          throw invalid_argument("Eingabe muss eine ganze Zahl sein.");
        }
        if (length < 1 || length > 9) {
          cout << "Bitte eine Zahl zwischen 1 und 9 eingeben" << endl;
        }
      } catch (const invalid_argument &e) {
        cout << e.what() << endl;
      }
    } while (length < 1 || length > 9);

    try {
      Flotten.at(i).resize(length);
    } catch (const out_of_range &) {
      throw runtime_error("Error: Out of range access to Flotten array.");
    }

    for (int j = 0; j < length; j++) {
      do {
        cout << "Schiff " << j + 1 << " von " << length << endl;
        cout << "(1)Jäger, (2)Zerstörer, 3)Kreuzer: ";
        try {
          cin >> type;
          if (cin.fail()) { // Check if the input is not an integer
            cin.clear();    // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Eingabe muss eine ganze Zahl sein.");
          }
          if (type < 1 || type > 3) {
            cout << "Bitte eine Zahl zwischen 1 und 3 eingeben" << endl;
          }
        } catch (const invalid_argument &e) {
          cout << e.what() << endl;
        }
      } while (type < 1 || type > 3);

      try {
        switch (type) {
        case 1:
          Flotten.at(i).at(j) = make_unique<Jaeger>();
          break;
        case 2:
          Flotten.at(i).at(j) = make_unique<Zerstoerer>();
          break;
        case 3:
          Flotten.at(i).at(j) = make_unique<Kreuzer>();
          break;
        default:
          break;
        }
      } catch (const out_of_range &) {
        throw runtime_error("Error: Out of range access to Flotten array.");
      }
    }
  }
  flottenInitialisieren();
}

void Spiel::flottenInitialisieren() {
  random_device rd;
  mt19937 gen(rd());

  // Use an array of size 2, where each element is a vector of C-style arrays
  // representing the positions of ships as {x, y} coordinates
  array<vector<array<int, 2>>, 2> placedShips;

  for (int player = 0; player < 2; ++player) {
    for (const auto &schiff : Flotten.at(player)) {
      bool isPlaced = false;

      while (!isPlaced) {
        int x, y;
        // Choose a random position between 0 and 9
        int gridSize = spielFeld->getGrid().size();
        uniform_int_distribution<> disX(0, gridSize - 1);
        x = disX(gen);
        y = disX(gen);

        // Check if the chosen position overlaps with any previously placed ship
        bool overlaps = false;
        for (const auto &placedShipPos : placedShips[player]) {
          // Check if both x and y coordinates match
          if (placedShipPos[0] == x && placedShipPos[1] == y) {
            overlaps = true;
            break;
          }
        }

        if (!overlaps) {
          array<int, 2> pair = {x, y};
          schiff->setX(x);
          schiff->setY(y);
          // Store the position as a C-style array {x, y}
          placedShips[player].push_back(pair);
          isPlaced = true;
        }
      }
    }
  }
}

Spiel::Spiel() { spielFeld = make_shared<Welt>(); }
