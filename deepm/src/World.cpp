#include "../inc/World.hpp"
#include <iostream>
#include <random>

using namespace std;

World::World(int depth) {
  Grid.resize(depth);
  setDepth(depth);
  setLength(5);
  setWidth(5);
  initWorld();
}

void World::initWorld() {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(-3, 9);

  for (int i = 0; i < getDepth(); i++) {
    for (int j = 0; j < getLength(); j++) {
      for (int k = 0; k < getWidth(); k++) {
        int value;
        do {
          value = dis(gen);
        } while (value == 0);
      }
    }
  }
}
