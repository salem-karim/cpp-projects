#include "Enemy/Enemy.h"
#include "Player/Player.h"
#include "World/World.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

void printWorld(World *world, Player *player, Enemy *enemy, Enemy *enemy2);

int main() {
  srand(time(0)); // Seed the random number generator
  Player *myPlayer = new Player(0, 0);
  World *myWorld = new World();
  Enemy *myEnemy = new Enemy();
  myEnemy->placeEnemy(myPlayer);
  Enemy *myEnemy2 = new Enemy();
  do {
    myWorld->initializeWorld(myPlayer);
  } while (!(myWorld->checkRelics()));
  string message = "", error = "", damage = "";
  char input = ' ';
  while (true) {
    system("clear");
    cout << "press x to end | press q to shoot Arrows" << endl;
    printWorld(myWorld, myPlayer, myEnemy, myEnemy2);
    cout << "Health: " << myPlayer->health << "| Score: " << myPlayer->score
         << "| Level: " << myWorld->size - 4 << endl
         << "Shields: " << myPlayer->shields << "| Arrows: " << myPlayer->arrows
         << endl;
    if (!error.empty() || !(message.empty())) {
      cout << damage << '|' << message << endl;
      cout << error << endl;
      message = "";
      error = "";
      damage = "";
    }
    cin >> input;

    if (input == 'q') {
      if (myPlayer->arrows == 0) {
        error = "You are out of arrows!";
        continue;
      }
      if (myWorld->size - 4 >= 5) {
        int whichEnemy;
        cout << "Which enemy do you want to shoot? (1 or 2): ";
        if (cin >> whichEnemy) {
          error = "Invalid input!";
          continue;
        }

        if (whichEnemy == 1)
          myPlayer->shootArrow(myWorld, myEnemy, error);
        else if (whichEnemy == 2)
          myPlayer->shootArrow(myWorld, myEnemy2, error);
      } else
        myPlayer->shootArrow(myWorld, myEnemy, error);
    } else if (myPlayer->move(myWorld, input, error)) {
      continue;
    }
    if ((myWorld->size - 4) >= 10 && myEnemy->turnsToRespawn == 0 &&
        myEnemy2->turnsToRespawn == 0) {
      damage = myEnemy->followPlayer(myWorld, myPlayer);
      damage = myEnemy->followPlayer(myWorld, myPlayer);
      damage = myEnemy2->followPlayer(myWorld, myPlayer);
      damage = myEnemy2->followPlayer(myWorld, myPlayer);
    } else if ((myWorld->size - 4) >= 5 && myEnemy->turnsToRespawn == 0 &&
               myEnemy2->turnsToRespawn == 0) {
      damage = myEnemy->followPlayer(myWorld, myPlayer);
      damage = myEnemy2->followPlayer(myWorld, myPlayer);
    } else if (myEnemy->turnsToRespawn == 0) {
      damage = myEnemy->followPlayer(myWorld, myPlayer);
    }
    myWorld->applyField(myPlayer, message);
    if (input == 'x' || myPlayer->health <= 0) {
      cout << "Game Over!"
           << " | Your Score was: " << myPlayer->score << endl;
      break;
    } else if (!(myWorld->checkRelics())) {
      myWorld->size++;

      do {
        myWorld->initializeWorld(myPlayer);
      } while (!(myWorld->checkRelics()));
    }
    myEnemy->decreaseRespawnCounter(myPlayer);
    if ((myWorld->size - 4) >= 5) {
      myEnemy2->decreaseRespawnCounter(myPlayer);
    }
  }
  delete myEnemy2;
  delete myEnemy;
  delete myWorld;
  delete myPlayer;
  return 0;
}

void printWorld(World *world, Player *player, Enemy *enemy, Enemy *enemy2) {
  for (int y = 0; y < world->size; ++y) {
    for (int x = 0; x < world->size; ++x) {
      cout << ' ';
      if (player->x == x && player->y == y) {
        if ((enemy->x == x && enemy->y == y) &&
            (enemy2->x == x && enemy2->y == y) && world->size - 4 >= 10) {
          cout << 'P' << 'E' << 'E';
        } else if ((enemy->x == x && enemy->y == y) ||
                   ((enemy2->x == x && enemy2->y == y) &&
                    world->size - 4 >= 5)) {
          cout << 'P' << 'E';
        } else
          cout << ' ' << 'P';
      } else if (enemy->x == x && enemy->y == y) {
        if (world->grid[x][y] != '-') {
          if ((enemy2->x == x && enemy2->y == y) && world->size - 4 >= 5)
            cout << 'E' << 'E' << world->grid[x][y];
          else
            cout << 'E' << world->grid[x][y];
        } else if ((enemy2->x == x && enemy2->y == y) && world->size - 4 >= 5)
          cout << 'E' << 'E';
        else
          cout << ' ' << 'E';
      } else if (enemy2->x == x && enemy2->y == y && world->size - 4 >= 5) {
        if (world->grid[x][y] != '-') {
          if (enemy->x == x && enemy->y == y)
            cout << 'E' << 'E' << world->grid[x][y];
          else
            cout << 'E' << world->grid[x][y];
        } else if (enemy->x == x && enemy->y == y)
          cout << 'E' << 'E';
        else
          cout << ' ' << 'E';
      } else {
        cout << ' ' << world->grid[x][y];
      }
    }
    cout << endl;
  }
}
