#include "authService.hpp"
#include <fstream>
#include <iostream>
#include <ldap.h>
#include <stdexcept>

void AuthService::loginUser(const std::string username,
                            const std::string password, const std::string &ip) {
  loadBlacklist(); // Ensure the latest blacklist is loaded
  if (isBlacklisted(ip)) {
    throw std::runtime_error("IP is blacklisted");
  }

  const std::string ldapBindUser =
      "uid=" + username + ",ou=people,dc=technikum-wien,dc=at";
  BerValue bindCredentials;
  bindCredentials.bv_val = (char *)password.c_str();
  bindCredentials.bv_len = password.size();
  BerValue *servercredp;

  int return_code = 0;
  return_code =
      ldap_sasl_bind_s(ldapHandle, ldapBindUser.c_str(), LDAP_SASL_SIMPLE,
                       &bindCredentials, nullptr, nullptr, &servercredp);
  if (return_code != LDAP_SUCCESS) {
    updateLoginAttempts(ip, false);
    throw std::runtime_error("LDAP login failed: " +
                             std::string(ldap_err2string(return_code)));
  }

  updateLoginAttempts(ip, true);
  std::cout << "LDAP login succeeded" << std::endl;
  user.isLoggedIn = true;
  user.username = username;
}

void AuthService::setupLDAP() {
  int return_code = 0;
  return_code = ldap_initialize(&ldapHandle, ldapUri);
  if (return_code != LDAP_SUCCESS) {
    throw std::runtime_error("ldap_init failed: " +
                             std::string(ldap_err2string(return_code)));
  }
  std::cout << "connected to LDAP server " << ldapUri << std::endl;
  return_code =
      ldap_set_option(ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion);
  if (return_code != LDAP_OPT_SUCCESS) {
    ldap_unbind_ext_s(ldapHandle, nullptr, nullptr);
    ldapHandle = nullptr; // Reset the LDAP handle
    throw std::runtime_error("ldap_set_option failed: " +
                             std::string(ldap_err2string(return_code)));
  }
}

AuthService::~AuthService() {
  if (ldapHandle) {
    ldap_unbind_ext_s(ldapHandle, nullptr, nullptr);
  }
  saveBlacklist();
}

User AuthService::getUser() { return user; }

bool AuthService::isBlacklisted(const std::string &ip) {
  auto it = blacklist.find(ip);
  if (it != blacklist.end()) {
    // Check if the current time is past the blacklist expiry time
    if (std::chrono::system_clock::now() < it->second) {
      return true; // Still blacklisted
    } else {
      // Blacklist expired, remove the entry
      blacklist.erase(it);
    }
  }
  return false;
}

void AuthService::updateLoginAttempts(const std::string &ip, bool success) {
  if (success) {
    loginAttempts.erase(ip);
  } else {
    loginAttempts[ip]++;
    if (loginAttempts[ip] >= 3) {
      blacklistIP(ip);
      loginAttempts.erase(ip);
    }
  }
}

void AuthService::blacklistIP(const std::string &ip) {
  // Set the blacklist expiry time to 1 minute from now
  blacklist[ip] = std::chrono::system_clock::now() + std::chrono::minutes(1);
  std::cout << "IP " << ip << " has been blacklisted for 1 minute."
            << std::endl;
  saveBlacklist(); // Save the updated blacklist
}

void AuthService::loadBlacklist() {
  std::lock_guard<std::mutex> lock(authMutex);

  // Attempt to open the file for reading
  std::ifstream infile(".blacklist.txt");

  // If the file does not exist, create it and return
  if (!infile.is_open()) {
    std::ofstream outfile(".blacklist.txt");
    if (!outfile.is_open()) {
      throw std::runtime_error("Failed to create .blacklist.txt");
    }
    outfile.close();
    return;
  }

  // Read the existing entries
  std::string ip;
  std::time_t expiryTime;
  while (infile >> ip >> expiryTime) {
    blacklist[ip] = std::chrono::system_clock::from_time_t(expiryTime);
  }
  infile.close();
}

void AuthService::saveBlacklist() {
  std::lock_guard<std::mutex> lock(authMutex);
  std::ofstream outfile(".blacklist.txt");
  if (!outfile.is_open()) {
    throw std::runtime_error("Failed to open .blacklist.txt for writing");
  }

  for (const auto &entry : blacklist) {
    auto expiryTime = std::chrono::system_clock::to_time_t(entry.second);
    outfile << entry.first << " " << expiryTime << std::endl;
  }
  outfile.close();
}
