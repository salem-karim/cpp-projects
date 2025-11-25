#include "server_socket.hpp"
#include "server_handler.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>

// initialize static members
ServerSocket *ServerSocket::instance = nullptr;
bool ServerSocket::abortRequested = false;

ServerSocket::ServerSocket(const int &port, std::string mail_directory)
    : mail_directory(std::move(mail_directory)), port(port) {
  instance = this;
  // Initialize the server socket struct
  int reuseValue = 1;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    throw std::runtime_error("socket creation failed");
  }
  // Set socket options to reuse address and port
  setSocketOption(SO_REUSEADDR, reuseValue);
  setSocketOption(SO_REUSEPORT, reuseValue);
  // Set up the server address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  // Bind and listen on the socket
  bindSocket();
  listenSocket();
}

void ServerSocket::setSocketOption(int option, int value) const {
  // Get String representation
  std::string option_name;
  switch (option) {
  case SO_REUSEADDR:
    option_name = "SO_REUSEADDR";
    break;
  case SO_REUSEPORT:
    option_name = "SO_REUSEPORT";
    break;
  default:
    break;
  } // set the option
  if (setsockopt(server_socket, SOL_SOCKET, option, &value, sizeof(value)) ==
      -1) {
    throw std::runtime_error("setsockopt failed for option " + option_name +
                             ": " + std::string(strerror(errno)));
  }
}

void ServerSocket::bindSocket() {
  // Try to bind the socket to the address and port
  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    throw std::runtime_error("bind failed on port " + std::to_string(port) +
                             ": " + std::string(strerror(errno)));
  }
}

void ServerSocket::listenSocket() const {
  // Try to listen on the socket
  if (listen(server_socket, 5) == -1) { // 5 is the backlog size
    throw std::runtime_error("listen failed on socket: " +
                             std::string(strerror(errno)));
  }
}

void ServerSocket::run() {
  while (!abortRequested) {
    // Accept incoming connections
    std::cout << "Waiting for incoming connections" << std::endl;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &addrlen);
    // Check if the server is shutting down
    if (client_socket == -1) {
      if (abortRequested) {
        std::cerr << "Server is shutting down gracefully: "
                  << std::strerror(errno) << std::endl;
      } else if (errno == EBADF) {
        std::cerr << "Server socket closed: " << std::strerror(errno)
                  << std::endl;
      } else {
        std::cerr << "accept error: " << std::strerror(errno) << std::endl;
      }
      break;
    }
    std::string client_ip = inet_ntoa(client_address.sin_addr);
    std::cout << "Client connected from " << client_ip << ":"
              << ntohs(client_address.sin_port) << "...\n";
    // Create a new thread to handle the client connection
    clientThreads.emplace_back([client_socket, client_ip, this]() {
      ServerHandler handler(&client_socket, client_ip, mail_directory);
      handler.handleCommunication();
    });
  }

  // Join all client threads before exiting
  for (auto &thread : clientThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

ServerSocket::~ServerSocket() {
  // Cleanup if needed
  if (server_socket != -1) {
    close(server_socket);
  }
}

void ServerSocket::signalHandler(int sig) {
  if (sig == SIGINT) { // Only respond to SIGINT
    std::cout << "Abort Requested due to SIGINT...\n";
    if (instance) {
      instance->handleShutdown(); // Clean up resources
    } else {
      std::cerr
          << "SIGINT received, but no ServerSocket instance is available.\n";
    }
  }
}

void ServerSocket::handleShutdown() {
  // Shutdown and close the client socket if open
  if (client_socket != -1) {
    if (shutdown(client_socket, SHUT_RDWR) == -1) {
      std::cerr << "Failed to shutdown client socket: " << std::strerror(errno)
                << std::endl;
    }
    if (close(client_socket) == -1) {
      std::cerr << "Failed to close client socket: " << std::strerror(errno)
                << std::endl;
    }
    client_socket = -1;
  }

  // Shutdown and close the server socket if open
  if (server_socket != -1) {
    if (shutdown(server_socket, SHUT_RDWR) == -1) {
      std::cerr << "Failed to shutdown server socket: " << std::strerror(errno)
                << std::endl;
    }
    if (close(server_socket) == -1) {
      std::cerr << "Failed to close server socket: " << std::strerror(errno)
                << std::endl;
    }
    server_socket = -1;
  }
}
