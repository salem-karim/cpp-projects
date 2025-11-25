#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>

using std::string, std::vector;

void findFile(vector<string> &results, const string &path, string file_name, const bool &recursive,
              const bool &case_insensitive);

#endif // SEARCH_H
