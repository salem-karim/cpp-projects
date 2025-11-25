#include "../inc/Game.h"
#include "../inc/Computer.h"
#include "../inc/Player.h"

#include <iostream>
#include <limits>

using namespace std;

Game::~Game() {
    for(const auto& elem : robots) {
        cout << "Score: " << elem->getScore() << endl;
    }
  delete world;
  delete robots[0];
  delete robots[1];
  cout << "Game object destroyed" << endl;
}

Game::Game() {
  int depth;
  while (true) {
    cout << "Enter the depth of the world: ";
    cin >> depth;
    if (!cin) {
      cout << "Bad value!" << endl;
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      continue;
    }
    break;
  }
  world = new World(depth);
  choosePlayers();
  gameLoop();
}

void Game::choosePlayers() {
  int choice;
  char type1, type2;

  while (true) {
    cout << "Chose 1. Player vs Player 2. Player vs Computer 3. Computer vs "
            "Computer: ";
    cin >> choice;
    if (!cin || (choice > 3 || choice < 1)) {
      cout << "Bad value!" << endl;
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      continue;
    }
    break;
  }

  while (true) {
    cout << "Choose the type of the first robot 1. Sorter(s) 2. Driller(d) 3. "
            "Risker(r): ";
    cin >> type1;
    if (type1 != 's' && type1 != 'd' && type1 != 'r') {
      cout << "Invalid input" << endl;
      continue;
    }

    cout << "Choose the type of the second robot 1. Sorter(s) 2. Driller(d) 3. "
            "Risker(r): ";
    cin >> type2;
    if (type2 != 's' && type2 != 'd' && type2 != 'r') {
      cout << "Invalid input" << endl;
      continue;
    }
    break;
  }

  switch (choice) {
  case 1:
    robots[0] = new Player(0, 0, 0, type1, world);
    robots[1] = new Player(0, 0, 0, type2, world);
    break;
  case 2:
    robots[0] = new Player(0, 0, 0, type1, world);
    robots[1] = new Computer(4, 4, 0, type2, world);
    break;
  case 3:
    robots[0] = new Computer(4, 4, 0, type1, world);
    robots[1] = new Computer(4, 4, 0, type2, world);
    break;
  }
}

void Game::gameRound() {
  char choice;
  for (int i = 0; i < 2; i++) {
    while (true) {
      world->printWorld(robots[0]->getX(), robots[0]->getY(), robots[1]->getX(),
                        robots[1]->getY());
      while (true) {
        gameCheck();
        cout << "Robot " << i + 1 << " turn" << endl;
        cout << "Do you want to (m)ove or (d)ig: ";
        cin >> choice;
        if (choice != 'm' && choice != 'd') {
          cout << "Invalid input" << endl;
          continue;
        } else
          break;
      }
      switch (choice) {
      case 'm':
        robots[i]->move();
        continue;
      case 'd':
        robots[i]->mine();
        for(const auto& elem : robots) {
            cout << "Score: " << elem->getScore() << endl;
        }
        break;
      }
      break;
    }
  }
}

void Game::gameLoop() {
  char choice;
  while (true) {
    while (true) {
      cout << "What do you want to do? (p)lay Round, (e)nd Game: ";
      cin >> choice;
      if (choice != 'p' && choice != 'e') {
        cout << "Invalid input" << endl;
        continue;
      } else if (choice == 'p') {
        break;
      } else {
        return;
      }
    }
    gameRound();
  }
}

void Game::gameCheck() {
  for (int i = 0; i < 2; i++) {
    if (robots[i]->getScore() > 0 && robots[i]->getScore() % 50 == 0)
      world->rerollWorld();
  }
  //if (world->getGrid().empty()) {
  //  cout << "Reshufling the world" << endl;
  //  world->rerollWorld();
  //}
}
