#include "../inc/Computer.h"
// #include <algorithm>
#include <random>

using namespace std;

Computer::Computer(int x, int y, int score, char type, World *world) {
  setX(x);
  setY(y);
  setScore(score);
  setType(type);
  setWorld(world);
}

void Computer::move() {
    // First create an instance of an engine.
    random_device rnd_device;
    // Specify the engine and distribution.
    mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    uniform_int_distribution<int> dist{1, 5};

    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

    int randomNumber = gen();
    int x = getX();
    int y = getY();
    switch (randomNumber) {
        case 1:
            if (y - 1 >= 0) {
                setY(y - 1);
            }
            break;
        case 2:
            if (y + 1 < 5) {
                setY(y + 1);
            }
            break;
        case 3:
            if (x - 1 >= 0) {
                setX(x - 1);
            }
            break;
        case 4:
            if (x + 1 < 5) {
                setX(x + 1);
            }
            break;
        case 5:
            break;
    }
}


