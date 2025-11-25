//
// Created by benni on 26.03.2024.
//

#include "hashTable.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

int hashTable::hashString(std::string input) {
  int size = input.length();
  int hash = 0;
  for (int i = 0; i < size; i++) {
    hash = hash + ((input[i] + input[i] ^ 3) * (i + 1));
  }
  hash = hash % LENGHT;
  return hash;
}

void hashTable::add(string name, string wkn, string krzl) {
  int hash_name = hashString(name);
  int hash_krzl = hashString(krzl);

  if (array_name[hash_name].name == name ||
      array_krzl[hash_krzl].krzl == krzl) {
    cout << "Aktie existiert bereits" << endl;
    return;
  }
  // if the hash is empty, add the stock
  if (array_name[hash_name].name.empty()) {
    array_name[hash_name].name = name;
    array_name[hash_name].wkn = wkn;
    array_name[hash_name].krzl = krzl;

  } else {
    // else use quadratic probing to find the next empty hash
    int i = 1;
    int new_hash = hash_name;
    while (!array_name[new_hash].name.empty()) {
      new_hash = (new_hash + i * i) % LENGHT;
      i++;
      if (new_hash == hash_name) {
        cout << "Hashliste voll!" << endl;
        return;
      }
    }
    array_name[new_hash].name = name;
    array_name[new_hash].wkn = wkn;
    array_name[new_hash].krzl = krzl;
    cout << "Aktie angelegt" << endl;
  }
  // the same for the krzl
  if (array_krzl[hash_krzl].krzl.empty()) {
    array_krzl[hash_krzl].krzl = krzl;
    array_krzl[hash_krzl].index_name = hash_name;
    return;
  } else {
    int i = 1;
    int new_hash = hash_krzl;
    while (!array_krzl[new_hash].krzl.empty()) {
      new_hash = (new_hash + i * i) % LENGHT;
      i++;
      if (new_hash == hash_krzl) {
        cout << "Hashliste voll!" << endl;
        return;
      }
    }
    array_krzl[new_hash].krzl = krzl;
    array_krzl[new_hash].index_name = hash_name;
    cout << "Aktie angelegt" << endl;
    return;
  }
}

void hashTable::del(int sel) {
  // set all values to empty or 0
  int hash_krzl = findHash(array_name[sel].krzl, 0);
  array_krzl[hash_krzl].krzl = "";
  array_krzl[hash_krzl].index_name = 0;
  array_name[sel].name = "";
  array_name[sel].wkn = "";
  array_name[sel].krzl = "";
  array_name[sel].name = "";
  day emptyDay = {};
  for (int i = 0; i < 30; i++) {
    // set all days to empty
    array_name[sel].werte[i] = emptyDay;
  }
}

void hashTable::imp() {
  string filename;
  string line;
  vector<string> last30Lines;
  for (int i = 0; i < LENGHT; i++) {
    if (!array_name[i].krzl.empty()) {
      cout << "Kuerzel: " << array_name[i].krzl << endl;
    }
  }
  cout << "Kuerzel Dateiname:";
  cin >> filename;
  // get the krzl from the filename
  stringstream ss(filename);
  string krzl;
  getline(ss, krzl, '.');
  int hash_krzl = findHash(krzl, 0);
  // if krzl is not found in the hash table return
  if (hash_krzl == -1) {
    cout << "Aktie nicht gefunden!" << endl;
    return;
  } else {
    // else get the last 30 lines from the file
    int index = array_krzl[hash_krzl].index_name;
    ifstream file(filename);
    if (file.is_open()) {
      while (getline(file, line)) {
        // add the lines to the vector and delete the first line if the vector
        // is longer than 30
        last30Lines.push_back(line);
        if (last30Lines.size() > 30) {
          last30Lines.erase(last30Lines.begin());
        }
      }
      for (int i = 0; i < last30Lines.size(); i++) {
        // parse the lines and add them to the hash table
        stringstream lineStream(last30Lines[i]);
        string cell;
        // get the data separated by commas and use stod and stoi to convert the
        // strings to double and int
        getline(lineStream, array_name[index].werte[i].date, ',');
        getline(lineStream, cell, ',');
        array_name[index].werte[i].open = stod(cell);
        getline(lineStream, cell, ',');
        array_name[index].werte[i].high = stod(cell);
        getline(lineStream, cell, ',');
        array_name[index].werte[i].low = stod(cell);
        getline(lineStream, cell, ',');
        array_name[index].werte[i].close = stod(cell);
        getline(lineStream, cell, ',');
        array_name[index].werte[i].adjClose = stod(cell);
        getline(lineStream, cell, ',');
        array_name[index].werte[i].volume = stoi(cell);
      }
      file.close();

    } else {
      cout << "Datei nicht gefunden!" << endl;
      return;
    }
  }
}

void hashTable::search() {
  char sel = ' ';
  string input;
  cout << "Mit (k)uerel oder (n)ame suchen?" << endl;
  while (sel != 'n' && sel != 'k') {
    cin >> sel;
  }
  cout << "Name oder Kuerzel?" << endl;
  cin >> input;
  int hash = 0;
  switch (sel) {
  case 'k':
    hash = findHash(input, 0);
    if (hash == -1) {
      cout << "Aktie nicht gefunden" << endl;
      return;
    }
    hash = array_krzl[hash].index_name;
    break;
  case 'n':
    hash = findHash(input, 1);
    if (hash == -1) {
      cout << "Aktie nicht gefunden" << endl;
      return;
    }
    break;
  default:
    break;
  }

  string day;
  string month;
  string year;
  cout << "Name: " << array_name[hash].name << endl;
  cout << "WKN: " << array_name[hash].wkn << endl;
  cout << "Kuerzerl: " << array_name[hash].krzl << endl;
  if (!array_name[hash].werte[29].date.empty()) {
  istringstream dateStream(array_name[hash].werte[29].date); // splits date into day month year
  getline(dateStream, year, '-');
  getline(dateStream, month, '-');
  getline(dateStream, day, '-');

  cout << "Datum: " << day << "." << month << "." << year << endl;
  cout << "Open: " << array_name[hash].werte[29].open << endl;
  cout << "High: " << array_name[hash].werte[29].high << endl;
  cout << "Low: " << array_name[hash].werte[29].low << endl;
  cout << "Close: " << array_name[hash].werte[29].close << endl;
  cout << "Volume: " << array_name[hash].werte[29].volume << endl;
  cout << "Adj Close: " << array_name[hash].werte[29].adjClose << endl;
  }
}

void hashTable::plot() {
  string input;
  for (int i = 0; i < LENGHT; i++) {
    if (!array_name[i].krzl.empty()) {
      cout << "Kuerzel: " << array_name[i].krzl << endl;
    }
  }
  cout << "Kuerzel: ";
  cin >> input;
  // find the hash of the input and check if the stock is empty or not found
  int hash = findHash(input, 0);
  if (hash == -1 ||
      array_name[array_krzl[hash].index_name].werte[0].close == 0) {
    cout << "Aktie ist leer oder wurde nicht gefunden!" << endl;
    return;
  }

  double close[30];
  char chart[60][15];
  // get the max and min close values
  double max_close = array_name[array_krzl[hash].index_name].werte[0].close;
  double min_close = max_close;
  for (int i = 0; i < 30; i++) {
    close[i] = array_name[array_krzl[hash].index_name].werte[i].close;
    if (close[i] > max_close) {
      max_close = close[i];
    }
    if (close[i] < min_close) {
      min_close = close[i];
    }
  }
  // calculate the scale factor
  double close_range = max_close - min_close;
  double scale_factor = 15.0 / close_range;
  // initialize the chart with spaces
  for (int i = 0; i < 60; i++) {
    for (int j = 0; j < 15; j++) {
      chart[i][j] = ' ';
    }
  }
  for (int i = 0; i < 30; i++) {
    // get the scaled close value
    int scaled_close = static_cast<int>((close[i] - min_close) * scale_factor);
    scaled_close = max(0, min(scaled_close, 14));
    // set the position of the star
    int y_position = 14 - scaled_close;
    int x_position = (i * 2) + 1;
    // check if the position is in bounds
    if (x_position >= 0 && x_position < 60 && y_position >= 0 &&
        y_position < 15) {
      chart[x_position][y_position] = '*';
    } else {
      cout << "Out-of-bounds position detected!" << endl;
      return;
    }
  }
  // print the chart
  cout << "Close Data for " << input
       << "(Read from left to right on a Scale of 15):" << endl;
  for (int j = 0; j < 15; j++) {
    for (int i = 0; i < 60; i++) {
      cout << chart[i][j];
    }
    cout << endl;
  }
}

void hashTable::load() {
  string filename;
  cout << "Filename: " << endl;
  cin >> filename;
  // open the file
  fstream MyCSV(filename, ios::in);
  string name;
  string wkn;
  string krzl;
  string zeile;
  for (int i = 0; i < LENGHT; ++i) {
    if (!array_name[i].name.empty()) {
      del(i);
    }
  }
  // skip the first line and read the rest of the file
  getline(MyCSV, zeile);
  zeile = "";
  while (getline(MyCSV, zeile)) {
    istringstream values(zeile);
    getline(values, name, ',');
    getline(values, wkn, ',');
    getline(values, krzl, ',');
    add(name, wkn, krzl);
  }
  cout << "Datei geladen" << endl;
}

void hashTable::save() {
  string filename;
  cout << "Filename: " << endl;
  cin >> filename;
  // create the file and write the data to it
  fstream MyCSV(filename, ios::out);
  if (!MyCSV.is_open()) {
    cout << "Datei konnte nicht geöffnet werden" << endl;
    return;
  }
  MyCSV << "Name,WKN,Krzl\n";
  for (int i = 0; i < 1009; ++i) {
    if (!array_name[i].name.empty()) {
      MyCSV << array_name[i].name + "," + array_name[i].wkn + "," +
                   array_name[i].krzl + "\n";
    }
  }
  MyCSV.close();
  cout << "Datei gespeichert." << endl;
}

int hashTable::findHash(std::string input, int sel) {
  int hash = hashString(input);
  switch (sel) {
  case 0:
    if (array_krzl[hash].krzl == input) {
      break;
    } else {
      int new_hash = 0;
      int i = 1;
      // use quadratic probing to find the right hash which matches the input
      while (array_krzl[hash].krzl != input) {
        new_hash = (new_hash + i * i) % LENGHT;
        i++;
        if (new_hash == hash) {
          return -1;
        }
      }
      hash = new_hash;
    }
    break;
  case 1:
    if (array_name[hash].name == input) {
      break;
    } else {
      int new_hash = 0;
      int i = 1;
      // the same here but with the other list
      while (array_name[hash].name != input) {
        new_hash = (new_hash + i * i) % LENGHT;
        i++;
        if (new_hash == hash) {
          return -1;
        }
      }
      hash = new_hash;
    }
  default:
    return -1;
  }
  return hash;
}
