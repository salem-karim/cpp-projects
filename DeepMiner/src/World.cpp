#include "../inc/World.h"
#include <algorithm>
#include <iostream>
#include <random>
#define EMPTY 10

using namespace std;

World::World(int depth) {
  Grid.resize(depth, vector<vector<int>>(5, vector<int>(5)));
  shuffleWorld();
}

World::~World() {
  Grid.clear();
  cout << "World deleted" << endl;
}

vector<layers> World::getGrid() const { return Grid; }

int World::getLastElement(int x, int y) const {
  if (x >= 0 && y >= 0 && x < Grid.size() && y < Grid[0].size()) {
    const auto &row = Grid[x][y]; // Get the row at position (x, y)
    if (!row.empty()) {
      return row.back(); // Return the last element of the row
    } else {
      std::cerr << "Row at position (" << x << ", " << y << ") is empty."
                << std::endl;
      // Return a default value or handle the empty row case as needed
      return 0; // For example, returning 0 as a default value
    }
  } else {
    std::cerr << "Invalid coordinates." << std::endl;
    // Return a default value or handle the out-of-bounds coordinates as needed
    return 0; // For example, returning 0 as a default value
  }
}

void World::shuffleWorld() {
  // First create an instance of a random device.
  random_device rnd_device;

  // Specify the engine and distribution.
  mt19937 mersenne_engine{rnd_device()};     // Generates random integers
  uniform_int_distribution<int> dist{-3, 9}; // Range from -3 to 9

  auto gen = [&dist, &mersenne_engine]() {
    int num;
    do {
      num = dist(mersenne_engine);
    } while (num == 0); // Continue generating until num is not 0
    return num;
  };

  // Generating random values for each element in vec
  for (auto &col : Grid) {
    for (auto &row : col) {
      generate(begin(row), end(row), gen);
    }
  }
}

void World::rerollWorld() {
  // First create an instance of an engine.
  random_device rnd_device;
  // Specify the engine and distribution.
  mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  uniform_int_distribution<int> dist{1, 3};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  int randomNumber = gen();
  switch (randomNumber) {
  case 1:
    shuffleWorld();
    break;
  case 2:
    sortWorld(true);
    break;
  case 3:
    sortWorld(false);
    break;
  }
}

void World::printWorld(int x1, int y1, int x2, int y2) {
    // Print top layer separately
    cout << "Layer 1: " << endl;
    printTopLayer(x1, y1, x2, y2);

    // Print remaining layers
    for (int layerIndex = 1; layerIndex < Grid.size(); ++layerIndex) {
        cout << "Layer " << layerIndex + 1 << ": " << endl;
        const auto &col = Grid[layerIndex];
        for (const auto &row : col) {
            for (const auto &elem : row) {
                if (elem > 0 && elem < 10)
                    cout << "[   " << elem << "  ]";
                else if (elem < 0)
                    cout << "[  " << elem << "  ]";
                else
                    cout << "[      ]";
            }
            cout << endl;
        }
        cout << endl;
    }
}


void World::printTopLayer(int x1, int y1, int x2, int y2) {
  for (int i = 0; i < Grid[0].size(); ++i) {
    for (int j = 0; j < Grid[0][i].size(); ++j) {
      if (Grid[0][i][j] != EMPTY && Grid[0][i][j] < 0) {
        if ((i == x1 && j == y1) && (i == x2 && j == y2))
          cout << "[p " << Grid[0][i][j] << "p2]";
        else if (i == x1 && j == y1)
          cout << "[p1" << Grid[0][i][j] << "  ]";
        else if (i == x2 && j == y2)
          cout << "[  " << Grid[0][i][j] << "p2]";
        else
          cout << "[  " << Grid[0][i][j] << "  ]";
      } else if (Grid[0][i][j] != EMPTY) {
        if ((i == x1 && j == y1) && (i == x2 && j == y2))
          cout << "[p  " << Grid[0][i][j] << "p2]";
        else if (i == x1 && j == y1)
          cout << "[p  " << Grid[0][i][j] << "  ]";
        else if (i == x2 && j == y2)
          cout << "[   " << Grid[0][i][j] << "p2]";
        else
          cout << "[   " << Grid[0][i][j] << "  ]";
      } else {
        if ((i == x1 && j == y1) || (i == x2 && j == y2))
          cout << "[p   p2]";
        else if (i == x1 && j == y1)
          cout << "[p     ]";
        else if (i == x2 && j == y2)
          cout << "[    p2]";
        else
          cout << "[      ]";
      }
    }
    cout << endl;
  }
  cout << endl;
}

void World::sortWorld(bool ascending) {
  for (auto &col : Grid) {
    for (auto &row : col) {
      if (ascending)
        sort(row.begin(), row.end());
      else
        sort(row.rbegin(), row.rend());
    }
  }
}

void World::sortPillar(int x, int y, bool ascending) {
  for (auto &col : Grid) {
    if (ascending)
      sort(col[x].begin(), col[x].end());
    else
      sort(col[x].rbegin(), col[x].rend());
  }
}
