#include "search.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

using std::string, std::vector, std::endl, std::cerr;
namespace fs = std::filesystem;

void findFile(vector<string> &results, const string &path, string file_name, const bool &recursive,
              const bool &case_insensitive) {
  // If case-insensitive change file name to lowercase
  if (case_insensitive) {
    transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);
  }
  // If its recursive it uses the recursive iterator otherwise the normal one
  try {
    if (recursive) {
      for (const auto &entry: fs::recursive_directory_iterator(path)) {
        string currentFile = entry.path().filename().string();
        if (case_insensitive) {
          // changes the current File it looks up to lower case to math with the input file name
          transform(currentFile.begin(), currentFile.end(), currentFile.begin(), ::tolower);
        }
        if (currentFile == file_name) {
          results.emplace_back(fs::absolute(entry.path()).string());
        }
      }
    } else {
      // Normal search with same change if case-insensitive
      for (const auto &entry: fs::directory_iterator(path)) {
        string currentFile = entry.path().filename().string();
        if (case_insensitive) {
          transform(currentFile.begin(), currentFile.end(), currentFile.begin(), ::tolower);
        }
        if (currentFile == file_name) {
          results.emplace_back(fs::absolute(entry.path()).string());
        }
      }
    }
  } catch (const fs::filesystem_error &e) {
    cerr << "Filesystem error: " << e.what() << endl;
  }
}
