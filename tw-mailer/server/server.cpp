#include "server_socket.hpp"
#include <csignal>
#include <iostream>
#include <string>
#include <sys/socket.h>

bool isValidPort(int port) { return port > 0 && port <= 65535; }
void printUsage(const char *programName);

int main(int argc, char **argv) {

  // Register signal handler for SIGINT
  if (signal(SIGINT, ServerSocket::signalHandler) == SIG_ERR) {
    perror("signal cannot be registered");
    exit(EXIT_FAILURE);
  }

  // Validate command line arguments
  if (argc != 3) {
    printUsage(argv[0]);
    return 1;
  }
  const int port = std::stoi(argv[1]); // Convert port from string to int

  // Validate port number
  if (!isValidPort(port)) {
    printUsage(argv[0]);
    std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
    return 1;
  }
  // Get mail directory
  const std::string mailDir = argv[2];
  // Try to start the server
  try {
    ServerSocket server(port, mailDir);
    server.run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void printUsage(const char *programName) {
  std::cerr << "Usage: " << programName << " <port> <mail-spool-directoryname>"
            << std::endl;
}
