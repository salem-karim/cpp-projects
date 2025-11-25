//
// Created by benni on 26.03.2024.
//

#ifndef HASHTABLE_DAY_H
#define HASHTABLE_DAY_H
#include <string>

struct day {
  std::string date = "";
  double open = 0;
  double high = 0;
  double low = 0;
  double close = 0;
  double adjClose = 0;
  int volume = 0;
};
#endif // HASHTABLE_DAY_H
