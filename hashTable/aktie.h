//
// Created by benni on 26.03.2024.
//

#ifndef HASHTABLE_AKTIE_H
#define HASHTABLE_AKTIE_H
#include "day.h"
#include <string>

using namespace std;

struct aktie {
  string name = "";
  string wkn = "";
  string krzl = "";
  day werte[30] = {};
};

#endif // HASHTABLE_AKTIE_H
