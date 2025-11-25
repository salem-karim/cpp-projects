#pragma once
#ifndef SERVER_HANDLER_HPP_
#define SERVER_HANDLER_HPP_

#include "authService.hpp"
#include "mailHandler.hpp"
#include <string>
#include <vector>

#define BUF 2048

class ServerHandler {
public:
  ServerHandler(const int *client_socket, const std::string &client_ip,
                const std::string &mail_directory);
  ~ServerHandler();
  void handleCommunication();
  bool sendMessage(int client_socket, const std::string &msg);
  bool sendError(int client_socket, const std::string &msg);
  static void parseRequest(const char *req, std::vector<std::string> &segments);
  void handleLogin(const std::vector<std::string> &segments);
  void handleSend(const std::vector<std::string> &segments);
  void handleList(const std::vector<std::string> &segments);
  void handleRead(const std::vector<std::string> &segments);
  void handleDelete(const std::vector<std::string> &segments);

private:
  const int *client_socket;
  int msgsize;
  char buffer[BUF];
  mailHandler handler;
  enum Command { LOGIN, SEND, LIST, READ, DELETE, QUIT, UNKNOWN };
  Command getCommand(const std::string &cmd);
  const std::string client_ip;
  AuthService ldapauth;
};

#endif
