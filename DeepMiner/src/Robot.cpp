#include "../inc/Robot.h"
#include <limits>
#include <random>
#define EMPTY 10

using namespace std;

int Robot::getX() const { return x; }
int Robot::getY() const { return y; }
int Robot::getScore() const { return score; }
char Robot::getType() const { return type; }
World Robot::getWorld() const { return *world; }

void Robot::setX(int x) { this->x = x; }
void Robot::setY(int y) { this->y = y; }
void Robot::setScore(int score) { this->score += score; }
void Robot::setType(char type) { this->type = type; }
void Robot::setWorld(World *world) { this->world = world; }

Robot::~Robot() { std::cout << "Robot destructor called" << std::endl; }

void Robot::removeLastElement(int x, int y) {
  // Check if the coordinates are within the bounds of the grid
  if (x >= 0 && y >= 0 && x < 5 && y < 5) {
    // Get the reference to the row specified by the player's coordinates
    vector<int> &row = world->getGrid()[x][y];

    // Check if the row is not empty before removing the last element
    if (!row.empty()) {
      row.pop_back(); // Remove the last element of the row
    } else {
      // Handle the case where the row is already empty
      std::cerr << "Cannot remove element from an empty row." << std::endl;
    }
  } else {
    // Handle the case where the coordinates are out of bounds
    std::cerr << "Invalid coordinates." << std::endl;
  }
}

void Robot::mine() {
  // Check if world pointer is initialized
  if (world == nullptr) {
    std::cerr << "Error: World pointer is not initialized." << std::endl;
    return;
  }

  // First create an instance of an engine.
  random_device rnd_device;
  // Specify the engine and distribution.
  mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  uniform_int_distribution<int> dist{0, 1};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  int randomNumber = gen();
  int randomNumber2 = gen();
  int x = getX();
  int y = getY();
  int value = world->getLastElement(x, y);
  cout << value << endl;
  cout << x << " " << y << endl;

  switch (getType()) {
  case 's':
    if (value > 0) {
      switch (randomNumber) {
      case 0:
        setScore(value);
        removeLastElement(x, y);
        cout << "Mined block with value: " << value << endl;
        break;
      case 1:
        world->sortPillar(getX(), getY(), true);
        value = world->getLastElement(getX(), getY());
        removeLastElement(x, y);
        setScore(value);
        cout << "Mined block with value: " << value << endl;
        break;
      }
    } else if (value == -1) {
      cout << "Robot is stuck" << endl;
    } else if (value == -2) {
      cout << "Robot is lost" << endl;
      moveToSmallest();
    } else if (value == -3) {
      switch (randomNumber) {
      case 0:
        setScore(value + 5);
        removeLastElement(x, y);
        cout << "Mined block with value plus extra 5 points: " << value << endl;
        break;
      case 1:
        world->shuffleWorld();
        cout << "Shuffled world" << endl;
        break;
      }
    }
    break;
  case 'd':
    if (value > 0) {
      switch (randomNumber) {
      case 0:
        setScore(value);
        removeLastElement(x, y);
        cout << "Mined block with value: " << value << endl;
        break;
      case 1:
        for (int i = 0; i < 3; i++) {
          setScore(value);
          removeLastElement(x, y);
          value = world->getLastElement(getX(), getY());
          cout << "Mined block with value: " << value << endl;
        }
        break;
      }
    } else if (value == -1) {
      cout << "Robot is stuck" << endl;
    } else if (value == -2) {
      cout << "Robot is lost" << endl;
      moveToSmallest();
    } else if (value == -3) {
      switch (randomNumber) {
      case 0:
        setScore(value + 5);
        removeLastElement(x, y);
        cout << "Mined block with value plus extra 5 points: " << value << endl;
        break;
      case 1:
        world->shuffleWorld();
        cout << "Shuffled world" << endl;
        break;
      }
    }
    break;
  case 'r':
    if (value > 0) {
      switch (randomNumber) {
      case 0:
        world->sortPillar(getX(), getY(), false);
        cout << "Sorted pillar descending" << endl;
        if (randomNumber2 == 0) {
          setScore(value / 2);
          removeLastElement(x, y);
          cout << "Mined block with half value: " << value << endl;
        } else {
          setScore(value * 2);
          removeLastElement(x, y);
          cout << "Mined block with double value: " << value << endl;
        }
        break;
      case 1:
        world->sortPillar(getX(), getY(), true);
        cout << "Sorted pillar ascending" << endl;
        if (randomNumber2 == 0) {
          setScore(value / 2);
          removeLastElement(x, y);
          cout << "Mined block with half value: " << value << endl;
        } else {
          setScore(value * 2);
          removeLastElement(x, y);
          cout << "Mined block with double value: " << value << endl;
        }
        break;
      }
    } else if (value == -1) {
      cout << "Robot is stuck" << endl;
    } else if (value == -2) {
      cout << "Robot is lost" << endl;
      moveToSmallest();
    } else if (value == -3) {
      switch (randomNumber) {
      case 0:
        setScore(value + 5);
        removeLastElement(x, y);
        cout << "Mined block with value: " << value << endl;
        break;
      case 1:
        world->shuffleWorld();
        cout << "Shuffled world" << endl;
        break;
      }
    }
    break;
  default:
    cout << "Invalid robot type." << endl;
    break;
  }
}

void Robot::moveToSmallest() {
  int smallest = numeric_limits<int>::max();
  int smallX = 0;
  int smallY = 0;
  for (int z = 0; z < world->getGrid().size(); z++) {
    for (int y = 0; y < world->getGrid()[z].size(); y++) {
      for (int x = 0; x < world->getGrid()[z][y].size(); x++) {
        if (world->getGrid()[z][y][x] < smallest &&
            world->getGrid()[z][y][x] > 0) {
          smallest = world->getGrid()[z][y][x];
          smallX = x;
          smallY = y;
        }
      }
    }
  }
  setX(smallX);
  setY(smallY);
  cout << "Moved to smallest block at position: (" << smallX << " | " << smallY
       << ")" << endl;
}
