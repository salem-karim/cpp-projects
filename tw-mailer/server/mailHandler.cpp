#include "mailHandler.hpp"
#include "uuid_v4.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

// for convenience
using json = nlohmann::json;

mailHandler::mailHandler(const std::string &mailDir) {
  // Combine current path with the mailDir
  std::filesystem::path fullPath = std::filesystem::current_path() / mailDir;
  this->mailDir = fullPath.string(); // Save the full path

  // Check if it's a valid path and a directory
  if (std::filesystem::exists(fullPath)) {
    if (std::filesystem::is_directory(fullPath)) {
      std::cout << "Directory already exists: " << fullPath << std::endl;
    } else {
      std::cerr << "Error: " << fullPath << " exists but is not a directory!"
                << std::endl;
      throw std::runtime_error("Invalid path: not a directory");
    }
  } else {
    // Create directory if it doesn't exist
    std::filesystem::create_directories(fullPath);
    std::cout << "Created directory: " << fullPath << std::endl;
  }
}

Mail mailHandler::getMail(const std::string &filename,
                          const std::string &username) const {
  // create Mail object from file contents mapped to json
  std::ifstream file(mailDir + "/" + username + "/" + filename);
  json j;
  file >> j;
  Mail mail;
  mail.filename = filename;
  mail.sender = j["sender"];
  mail.subject = j["subject"];
  mail.body = j["body"];
  mail.timestamp = j["timestamp"];
  return mail;
}

void mailHandler::writeMail(Mail mail, const std::string &username) const {
  // scope the 2 lock guards in case where the user directory does not exist
  // Meaning one thread which sends to same user waits for first to create dir
  {
    // Lock the mutex for this user's directory
    std::lock_guard<std::mutex> lock(userLocks[username]);

    std::string userDir = mailDir + "/" + username;
    if (!std::filesystem::exists(userDir)) {
      std::filesystem::create_directories(userDir);
      std::cout << "Created user directory: " << userDir << std::endl;
    }
  }
  // Automatic deconstruction and unlock of mutex since it left scope

  UUIDv4::UUID uuid = uuidGenerator.getUUID();
  mail.filename = uuid.str() + ".json";

  // Second scope for writing Mail to the user dir
  {
    std::lock_guard<std::mutex> lock(userLocks[username]);
    std::ofstream file(mailDir + "/" + username + "/" + mail.filename);
    if (file.is_open()) {
      json j = {{"sender", mail.sender},
                {"subject", mail.subject},
                {"body", mail.body},
                {"timestamp", mail.timestamp}};
      file << j;
      std::cout << "Mail written to file: " << mail.filename << std::endl;
    } else {
      throw std::runtime_error("Failed to open file: " + mail.filename);
    }
  }
}

std::vector<std::string>
mailHandler::listMail(const std::string &username) const {
  // Lock only the user's directory, allowing threads to access different
  // users' mail directories in parallel
  std::lock_guard<std::mutex> lock(userLocks[username]);

  std::vector<std::string> mailList;
  std::filesystem::path userDir = mailDir + "/" + username;

  if (!std::filesystem::exists(userDir)) {
    std::cerr << "Directory does not exist: " << userDir << std::endl;
    return mailList;
  }

  // Collect filenames and sort them for consistent order across operations
  std::vector<std::string> filenames;
  for (const auto &entry : std::filesystem::directory_iterator(userDir)) {
    if (entry.is_regular_file()) {
      filenames.emplace_back(entry.path().filename().string());
    }
  }

  // Sort filenames to ensure consistent ordering across methods like
  // deleteMailByIndex and getMailByIndex
  std::sort(filenames.begin(), filenames.end());

  // Only get the mail subjects as specified in the protocol
  for (const auto &filename : filenames) {
    mailList.emplace_back(getMail(filename, username).subject);
  }

  return mailList;
}

Mail mailHandler::getMailByIndex(const size_t index,
                                 const std::string &username) const {
  // No mutex needed since filesystem reading is thread safe
  // And all the write / delete operations are synchronized
  std::vector<std::string> filenames;
  std::filesystem::path userDir = mailDir + "/" + username;

  try {
    // Iterate over the files and collect filenames
    for (const auto &entry : std::filesystem::directory_iterator(userDir)) {
      filenames.emplace_back(entry.path().filename().string());
    }
  } catch (const std::filesystem::filesystem_error &e) {
    throw std::runtime_error("Could not open directory: " +
                             std::string(e.what()));
  }

  // Sort the filenames to ensure consistent order
  std::sort(filenames.begin(), filenames.end());

  // Check if index is valid
  if (index >= filenames.size()) {
    throw std::out_of_range("Invalid mail index");
  }

  // Get mail by filename
  return getMail(filenames[index], username);
}

void mailHandler::deleteMailByIndex(size_t index,
                                    const std::string &username) const {
  // Mutex is needed since its a write / delete operations
  // Making read operations thread safe
  std::lock_guard<std::mutex> lock(userLocks[username]);

  std::vector<std::string> filenames;
  std::filesystem::path userDir = mailDir + "/" + username;

  for (const auto &entry : std::filesystem::directory_iterator(userDir)) {
    filenames.emplace_back(entry.path().filename().string());
  }

  std::sort(filenames.begin(), filenames.end());
  if (index >= filenames.size()) {
    throw std::out_of_range("Invalid mail index");
  }

  std::filesystem::remove(userDir / filenames[index]);
}
