#include "client_socket.hpp"
#include <arpa/inet.h> // For inet_pton
#include <iostream>

bool isValidPort(int port) { return port > 0 && port <= 65535; }
bool isValidIP(const std::string &ip);
void printUsage(const std::string &programName);

int main(int argc, char *argv[]) {
  // Check command line arguments
  if (argc != 3) {
    printUsage(argv[0]);
    return 1;
  }
  // Get the Address and Port from the command line arguments
  std::string ip = argv[1];
  int port = std::stoi(argv[2]);

  // Validate IP address
  if (!isValidIP(ip)) {
    std::cerr << "Error: Invalid IP address: " << ip << std::endl;
    printUsage(argv[0]);
    return 1;
  }

  // Validate port number
  if (!isValidPort(port)) {
    std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
    printUsage(argv[0]);
    return 1;
  }
  // Try to connect to the server
  try {
    ClientSocket client(ip, port);
    client.connectToServer();
    client.commandLoop(); // Start the command loop
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

bool isValidIP(const std::string &ip) {
  struct sockaddr_in sa;
  // Use inet_pton to validate the IP address
  return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
}

void printUsage(const std::string &programName) {
  std::cerr << "Usage: " << programName << " <ip> <port>" << std::endl;
}
