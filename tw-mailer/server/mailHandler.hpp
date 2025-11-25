//
// Created by sera on 10/19/24.
//

#ifndef MAILHANDLER_HPP
#define MAILHANDLER_HPP
#include "Mail.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class mailHandler {
public:
  explicit mailHandler(const std::string &mailDir);
  Mail getMail(const std::string &filename, const std::string &username) const;
  Mail getMailByIndex(const size_t index, const std::string &username) const;
  void writeMail(Mail mail, const std::string &username) const;
  std::vector<std::string> listMail(const std::string &username) const;
  void deleteMailByIndex(const size_t index, const std::string &username) const;

private:
  std::string mailDir;
  // Mutex map for user directories
  mutable std::unordered_map<std::string, std::mutex> userLocks;
};

#endif // MAILHANDLER_HPP
