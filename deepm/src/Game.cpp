#include "../inc/Game.hpp"
#include <iostream>
#include <limits>

using namespace std;

Game::Game() {
  startGame();
  choosePlayers();
  gameLoop();
}

void Game::startGame() {
  int depth;
  do {
    cout << "Enter the depth of the world: ";
    cin >> depth;
    if (depth < 1 || !cin) {
      cout << "Depth must be a Integer greater than 0." << endl;
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
  } while (depth < 1 || !cin);
  world = new World(depth);
}

Game::~Game() {
  std::cout << "Game Over!" << std::endl;
  // std::cout << "Robot 1: " << players[0]->getScore() << std::endl;
  // std::cout << "Robot 2: " << players[1]->getScore() << std::endl;
  delete world;
  // for (const auto &player : players) {
  //   delete player;
  // }
}

// void Game::choosePlayers(){}

void Game::gameLoop() {
  char choice;
  do {
    do {
      cout << "Do you want to (p)lay or (q)uit? ";
      cin >> choice;
      if (choice != 'p' && choice != 'q')
        cout << "Invalid Input!" << endl;
    } while (choice != 'p' && choice != 'q');
    if (choice == 'p')
      gameRound();
  } while (choice != 'q');
}
