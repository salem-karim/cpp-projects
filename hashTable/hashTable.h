//
// Created by benni on 26.03.2024.
//

#ifndef HASHTABLE_HASHTABLE_H
#define HASHTABLE_HASHTABLE_H
#define LENGHT 1009
#include "aktie.h"
#include "aktie_krzl.h"
#include <string>

class hashTable {
public:
  int hashString(std::string input);
  int findHash(std::string input, int sel);
  void add(std::string name, std::string wkn, std::string krzl);
  void del(int sel);
  void imp();
  void search();
  void plot();
  void load();
  void save();
  aktie array_name[LENGHT];
  aktie_krzl array_krzl[LENGHT];
};

#endif // HASHTABLE_HASHTABLE_H
