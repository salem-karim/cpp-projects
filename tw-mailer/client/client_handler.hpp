#pragma once
#ifndef CLIENT_HANDLER_HPP_
#define CLIENT_HANDLER_HPP_

#include "client_socket.hpp"
#include <string>

class ClientHandler {
public:
  explicit ClientHandler(ClientSocket *client);
  void executeCommand();

private:
  ClientSocket *client; // Pointer to the ClientSocket

  enum Command { LOGIN, SEND, LIST, READ, DELETE, UNKNOWN, QUIT };

  Command getCommand(const std::string &cmd); // Command mapping
  void handleLogin();
  void handleSend();
  void handleList();
  void handleRead();
  void handleDelete();
  void printPrompt();
  const std::string getpass();
  int getch();
  bool isLoggedIn;
};

#endif
