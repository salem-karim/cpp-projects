#pragma once
#ifndef SERVER_SOCKET_HPP_
#define SERVER_SOCKET_HPP_

#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>

class ServerSocket {
public:
  ServerSocket(const int &port, std::string mail_directory);
  ~ServerSocket();
  static bool abortRequested;
  void run();
  static void signalHandler(int sig);
  void handleShutdown();

private:
  int server_socket = -1;
  int client_socket = -1;
  const std::string mail_directory;
  const int port;
  struct sockaddr_in server_address {
  }, client_address{};
  static ServerSocket *instance;
  std::vector<std::thread> clientThreads;
  void setSocketOption(int option, int value) const;
  void bindSocket();
  void listenSocket() const;
};

#endif
