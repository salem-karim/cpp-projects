#include "aktie.h"
#include "hashTable.h"
#include <iostream>
#include <limits>
#ifdef _WIN32
#define CLEAR "cls"
#else // In any other OS
#define CLEAR "clear"
#endif

using namespace std;
void printMenu();

int main() {
  hashTable *table = new hashTable;
  string name;
  string wkn;
  string krzl;
  int sel;
  int counter = 0;

  while (true) {
    printMenu();
    char choice;
    cin >> choice;
    switch (choice) {
    case 'a':
      system(CLEAR);
      cout << "Name: ";
      cin >> name;
      cout << "WKN: ";
      cin >> wkn;
      cout << "KRZL: ";
      cin >> krzl;
      table->add(name, wkn, krzl);
      break;
    case 's':
      system(CLEAR);
      table->search();
      break;
    case 'd':
      system(CLEAR);
      for (int i = 0; i < LENGHT; i++) {
        if (!table->array_name[i].name.empty()) {
          cout << i << ": "
               << "Name: " << table->array_name[i].name << endl;
          counter++;
        }
      }
      if (counter == 0) {
        cout << "Keine Aktien gespeichert!" << endl;
        break;
      }
      cout << "Welche Aktie soll geloescht werden? "
           << "0-1008" << endl;
      if (!(cin >> sel)) {
        cout << "Ungueltige Eingabe!" << endl;
        // clears input buffer and ignores the rest of the input buffer until a
        // newline character is found
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
      }
      if (sel < 0 || sel >= LENGHT || table->array_name[sel].name.empty()) {
        cout << "Ungueltiger Index!" << endl;
        break;
      }
      table->del(sel);
      cout << "Aktie geloescht!" << endl;
      counter = 0;
      break;
    case 'p':
      system(CLEAR);
      table->plot();
      break;
    case 'i':
      system(CLEAR);
      table->imp();
      break;
    case 'S':
      system(CLEAR);
      table->save();
      break;
    case 'l':
      system(CLEAR);
      table->load();
      break;
    case 'q':
      delete table;
      return 0;
    default:
      cout << "Ungueltige Eingabe" << endl;
    }
  }
}

void printMenu() {
  cout << "(a)dd, (s)earch, (d)elete, (p)lot, (i)mport, (S)ave, (l)oad, "
          "(q)uit:";
}
