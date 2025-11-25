#include "client_socket.hpp"
#include "client_handler.hpp"
#include <cstring> // For memset
#include <iostream>
#include <stdexcept> // For exception handling
#include <unistd.h>  // For close
#include <utility>
#include <vector>

#define BUFF 512

ClientSocket::ClientSocket(std::string ip, const int &port)
    : port(port), server_ip(std::move(ip)) {
  // Create the socket
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    throw std::runtime_error("Socket creation failed");
  }

  // Initialize the server address structure
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  // Convert IP address
  if (inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr) <= 0) {
    throw std::runtime_error("Invalid IP address");
  }
}

void ClientSocket::connectToServer() {
  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    throw std::runtime_error("Connection to server failed");
  }
  std::cout << "Connected to server at " << server_ip << ":" << port
            << std::endl;
}

void ClientSocket::sendMessage(const std::string &message) const {
  send(client_socket, message.c_str(), message.size(), 0);
}

const std::string ClientSocket::receiveMessage() const {
  std::string message;            // Stores the complete message
  std::vector<char> buffer(BUFF); // Temporary buffer for each chunk

  while (true) {
    // Receive a chunk of data
    ssize_t size = recv(client_socket, buffer.data(), buffer.size(), 0);

    if (size > 0) {
      // Append received data to the message string
      message.append(buffer.data(), size);

      // If size < chunk_size, assume the message is complete
      if (size < BUFF) {
        break;
      }
    } else if (size == 0) {
      // Connection closed by the server
      break;
    } else {
      // Error in receiving data
      return "Failed to receive message from server\n";
    }
  }

  // Output the complete message
  return "Server response: " + message + '\n';
}

void ClientSocket::commandLoop() {
  ClientHandler handler(this);
  handler.executeCommand();
}

ClientSocket::~ClientSocket() {
  close(client_socket); // Close the socket on destruction
  std::cout << "Socket closed" << std::endl;
}
