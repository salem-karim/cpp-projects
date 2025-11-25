#pragma once
#ifndef CLIENT_SOCKET_HPP_
#define CLIENT_SOCKET_HPP_

#include <arpa/inet.h>
#include <string>

class ClientSocket {
public:
  ClientSocket(std::string ip, const int &port);
  void connectToServer();
  void sendMessage(const std::string &message) const;
  void commandLoop();
  const std::string receiveMessage() const;
  ~ClientSocket();

private:
  int client_socket;
  int port;
  struct sockaddr_in server_address;
  std::string server_ip;
};

#endif
