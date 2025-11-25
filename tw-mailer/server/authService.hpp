#pragma once
#ifndef _AUTHSERVICE_HPP_
#define _AUTHSERVICE_HPP_

#include <chrono> // For time-based blacklisting
#include <ldap.h>
#include <mutex>
#include <string>
#include <unordered_map>

typedef struct {
  std::string username;
  bool isLoggedIn;
} User;

class AuthService {
public:
  void loginUser(const std::string username, const std::string password,
                 const std::string &ip);
  bool isBlacklisted(const std::string &ip);
  User getUser();
  ~AuthService();
  void setupLDAP();
  void loadBlacklist();
  void saveBlacklist();

private:
  void updateLoginAttempts(const std::string &ip, bool success);
  void blacklistIP(const std::string &ip);

  User user = {"", false};
  const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
  const int ldapVersion = LDAP_VERSION3;
  LDAP *ldapHandle;

  std::unordered_map<std::string, int>
      loginAttempts; // Key: ip, Value: attempt count
  std::unordered_map<std::string, std::chrono::system_clock::time_point>
      blacklist; // Key: ip, Value: expiry time
  std::mutex authMutex;
};

#endif
