#include "server_handler.hpp"
#include "mailHandler.hpp"
#include "server_socket.hpp"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

ServerHandler::ServerHandler(const int *client_socket,
                             const std::string &client_ip,
                             const std::string &mail_directory)
    : client_socket(client_socket), handler(mail_directory),
      client_ip(client_ip), ldapauth() {
  ldapauth.setupLDAP();
}

void ServerHandler::handleCommunication() {
  do {
    // Receive message from client
    msgsize = recv(*client_socket, buffer, BUF - 1, 0);
    // Check if message was received, if not, print error message
    if (msgsize == -1) {
      if (ServerSocket::abortRequested) {
        std::cerr << "Error: Failed to receive message after abort"
                  << std::endl;
      } else {
        std::cerr << "Error: Failed to receive message" << std::endl;
      }
      break;
    }
    // Check if client closed remote socket
    if (msgsize == 0) {
      std::cout << "Client closed remote socket" << std::endl;
      break;
    }
    // remove ugly debug message, because of the sent newline of client
    if (buffer[msgsize - 2] == '\r' && buffer[msgsize - 1] == '\n') {
      msgsize = 2;
    } else if (buffer[msgsize - 1] == '\n') {
      --msgsize;
    }
    // Add null terminator
    buffer[msgsize] = '\0';
    // Parse the request into segments
    std::vector<std::string> segments;
    parseRequest(buffer, segments);
    Command cmd = getCommand(segments.at(0));
    // Handle the request based on the command
    if (!ldapauth.getUser().isLoggedIn) {
      switch (cmd) {
      case LOGIN:
        handleLogin(segments);
        break;
      case QUIT:
        return;
      default:
        sendError(*client_socket, "Please log in first");
      }
    } else {
      switch (cmd) {
      case SEND:
        handleSend(segments);
        break;
      case LIST:
        handleList(segments);
        break;
      case READ:
        handleRead(segments);
        break;
      case DELETE:
        handleDelete(segments);
        break;
      case QUIT:
        return;
      default:
        sendError(*client_socket, "Invalid command");
      }
    }
  } while (!ServerSocket::abortRequested);
}

ServerHandler::~ServerHandler() {
  // Close the client socket
  if (client_socket != nullptr) {
    close(*client_socket);
  }
}

void ServerHandler::parseRequest(const char *req,
                                 std::vector<std::string> &segments) {
  std::string request(req);
  std::istringstream stringStream(request);
  std::string segment;

  // Estimate number of segments
  int estimatedSegments =
      std::count(req, req + strlen(req), '\n') + 1; // count newlines
  segments.reserve(estimatedSegments); // Reserve space for segments

  // Split the request into segments based on newline using a stringstream
  while (std::getline(stringStream, segment)) {
    segment.erase(segment.find_last_not_of(" \n\r\t") +
                  1); // Trim trailing whitespace
    if (!segment.empty()) {
      segments.emplace_back(std::move(segment)); // Use segment (not request)
    }
  }
}

bool ServerHandler::sendMessage(int client_socket, const std::string &msg) {
  size_t total_sent = 0;
  while (total_sent < msg.size()) {
    ssize_t bytes_sent = send(client_socket, msg.c_str() + total_sent,
                              msg.size() - total_sent, 0);
    if (bytes_sent == -1) {
      std::cerr << "Error: Failed to send message" << std::endl;
      return false;
    }
    total_sent += bytes_sent;
  }
  return true;
}

bool ServerHandler::sendError(int client_socket, const std::string &msg) {
  if (send(client_socket, "ERR\n", 3, 0) == -1) {
    std::cerr << "Error: Failed to send error message" << std::endl;
    return false;
  }
  std::cerr << "Error: " << msg << std::endl;
  return true;
}

void ServerHandler::handleLogin(const std::vector<std::string> &segments) {
  if (segments.size() != 3) {
    sendError(*client_socket, "Invalid number of arguments for login");
    return;
  }

  std::string username = segments.at(1);
  std::string password = segments.at(2);

  try {
    ldapauth.loginUser(username, password, client_ip);
    sendMessage(*client_socket, "OK\n");
  } catch (const std::exception &e) {
    sendError(*client_socket, "Login failed: " + std::string(e.what()));
  }
}

void ServerHandler::handleSend(const std::vector<std::string> &segments) {
  // Check if the number of arguments is valid
  if (segments.size() < 4) {
    sendError(*client_socket, "Invalid number of arguments");
    return;
  } // Create a new mail object from the segments
  Mail toSend;
  toSend.filename = "";
  toSend.sender = ldapauth.getUser().username;
  toSend.subject = segments.at(2);
  // Concatenate the message till ".\n"
  std::string message;
  for (size_t i = 3; i < segments.size(); ++i) {
    message += segments.at(i);
    message += '\n';
  }
  toSend.body = message;
  toSend.timestamp = std::time(nullptr);
  // Write the mail to the file and send a response to the client
  handler.writeMail(toSend, segments.at(1));
  sendMessage((*client_socket), "OK\n");
}

void ServerHandler::handleList(const std::vector<std::string> &segments) {
  // Check if the number of arguments is valid
  if (segments.size() != 1) {
    sendError(*client_socket, "Invalid number of arguments");
    return;
  } // get the list of subjects from the mail directory
  std::vector<std::string> subjects =
      handler.listMail(ldapauth.getUser().username);
  if (subjects.empty()) {
    sendError(*client_socket, "No mail found");
    return;
  } // Make a response message with the list of subjects
  std::string msg = std::to_string(subjects.size()) + "\n";
  for (const auto &subject : subjects) {
    msg += subject + "\n";
  } // Send the response to the client
  sendMessage((*client_socket), msg + "\n");
}

void ServerHandler::handleRead(const std::vector<std::string> &segments) {
  // Check if the number of arguments is valid
  if (segments.size() != 2) {
    sendError(*client_socket, "Invalid number of arguments");
    return;
  }
  // Get the mail object by index and send the mail details to the client
  try {
    size_t msgIndex =
        std::stoul(segments.at(1)) - 1; // Convert msg number to index
    Mail mail = handler.getMailByIndex(msgIndex, ldapauth.getUser().username);
    // Create a response message with the mail
    std::string response = "OK\nSender: " + mail.sender +
                           "\nSubject: " + mail.subject +
                           "\nBody: " + mail.body + "\n";
    sendMessage(*client_socket, response);
    // Catch exceptions and send error messages
  } catch (const std::out_of_range &) {
    sendError(*client_socket, "Mail index out of range");
  } catch (const std::invalid_argument &) {
    sendError(*client_socket, "Invalid mail index format");
  } catch (const std::filesystem::filesystem_error &e) {
    sendError(*client_socket, "Filesystem error: " + std::string(e.what()));
  } catch (const std::exception &e) {
    sendError(*client_socket, "An error occurred: " + std::string(e.what()));
  }
}

void ServerHandler::handleDelete(const std::vector<std::string> &segments) {
  // Check if the number of arguments is valid
  if (segments.size() != 2) {
    sendError(*client_socket, "Invalid number of arguments");
    return;
  }
  // Delete the mail object by index and send a response to the client
  try {
    size_t msgIndex =
        std::stoul(segments.at(1)) - 1; // Convert msg number to index
    handler.deleteMailByIndex(msgIndex, ldapauth.getUser().username);
    sendMessage(*client_socket, "OK\n");
    // Catch exceptions and send error messages
  } catch (const std::out_of_range &) {
    sendError(*client_socket, "Mail index out of range");
  } catch (const std::invalid_argument &) {
    sendError(*client_socket, "Invalid mail index format");
  } catch (const std::filesystem::filesystem_error &e) {
    sendError(*client_socket, "Filesystem error: " + std::string(e.what()));
  } catch (const std::exception &e) {
    sendError(*client_socket, "An error occurred: " + std::string(e.what()));
  }
}

ServerHandler::Command ServerHandler::getCommand(const std::string &cmd) {
  // Map the command string to the corresponding enum value
  if (cmd == "SEND") {
    return SEND;
  } else if (cmd == "LIST") {
    return LIST;
  } else if (cmd == "READ") {
    return READ;
  } else if (cmd == "DEL") {
    return DELETE;
  } else if (cmd == "QUIT") {
    return QUIT;
  } else if (cmd == "LOGIN") {
    return LOGIN;
  } else {
    return UNKNOWN;
  }
}
