#include "../inc/Player.h"

using namespace std;

Player::Player(int x, int y, int score, char type, World *world) {
  setX(x);
  setY(y);
  setScore(score);
  setType(type);
  setWorld(world);
}

void Player::move() {
  char direction;
  int x = getX();
  int y = getY();
  while (true) {
    cout << "Current position: (" << x << " | " << y << ")" << endl;
    cout << "Enter a direction (w, a, s, d) or (r)est: ";
    cin >> direction;
    switch (direction) {
    case 'w':
      if (x > 0 && x < 5) {
        setX(x - 1);
        return;
      }
      cout << "You can't go further up." << endl;
      break;
    case 'a':
      if (y > 0 && y < 5) {
        setY(y - 1);
        return;
      }
      cout << "You can't go further left." << endl;
      break;
    case 's':
      if (x >= 0 && x < 4) {
        setX(x + 1);
        return;
      }
      cout << "You can't go further down." << endl;
      break;
    case 'd':
      if (y >= 0 && y < 4) {
        setY(y + 1);
        return;
      }
      cout << "You can't go further right." << endl;
      break;
    case 'r':
      return;
    default:
      cout << "Invalid input. Please try again." << endl;
      break;
    }
  }
}
