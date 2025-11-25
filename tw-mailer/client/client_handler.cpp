#include "client_handler.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#define SUBJ 80

ClientHandler::ClientHandler(ClientSocket *client)
    : client(client), isLoggedIn(false) {}

ClientHandler::Command ClientHandler::getCommand(const std::string &cmd) {
  if (cmd == "SEND") {
    return SEND;
  } else if (cmd == "LIST") {
    return LIST;
  } else if (cmd == "READ") {
    return READ;
  } else if (cmd == "LOGIN") {
    return LOGIN;
  } else if (cmd == "DEL") {
    return DELETE;
  } else if (cmd == "QUIT") {
    return QUIT;
  } else {
    return UNKNOWN;
  }
}

void ClientHandler::executeCommand() {
  std::string buffer = "";
  bool isQuit = false;
  // main Loop where the commands are read and mapped to correct Methods
  do {
    std::cout << ">> "; // Print prompt
    if (std::getline(std::cin, buffer)) {
      Command cmd = getCommand(buffer); // Map string to enum
      if (!isLoggedIn) {
        switch (cmd) {
        case LOGIN:
          handleLogin();
          break;
        case QUIT:
          isQuit = true;
          break;
        case UNKNOWN:
        default:
          std::cout << "Unknown command." << std::endl;
          break;
        }
      } else {
        switch (cmd) {
        case SEND:
          handleSend();
          break;
        case LIST:
          handleList();
          break;
        case READ:
          handleRead();
          break;
        case DELETE:
          handleDelete();
          break;
        case QUIT:
          isQuit = true;
          break;
        case UNKNOWN:
        default:
          std::cout << "Unknown command." << std::endl;
          break;
        }
      }
    } else {
      perror("Error getting input");
      isQuit = true; // Exit loop on error
    }
  } while (!isQuit);

  std::cout << "Exiting command loop." << std::endl;
}

void ClientHandler::handleLogin() {
  std::string username = "", password = "";

  std::cout << "LDAP Username: ";
  if (std::getline(std::cin, username)) {
    username.erase(0, username.find_first_not_of(" \t\n\r\f\v"));
    username.erase(username.find_last_not_of(" \t\n\r\f\v") + 1);
  } else {
    std::cerr << "Error getting username" << std::endl;
    return;
  }

  std::cout << "Password: ";
  password = getpass();

  const std::string buffer = "LOGIN\n" + username + '\n' + password + '\n';
  client->sendMessage(buffer);

  const std::string response = client->receiveMessage();
  std::cout << response;
  if (response.find("OK") != std::string::npos) {
    isLoggedIn = true;
  }
}

void ClientHandler::handleSend() {
  std::string receiver = "", subject = "", message = "";

  // Get receiver and do the same string manipulation
  std::cout << "Receiver: ";
  if (std::getline(std::cin, receiver)) {
    receiver.erase(0, receiver.find_first_not_of(" \t\n\r\f\v"));
    receiver.erase(receiver.find_last_not_of(" \t\n\r\f\v") + 1);
  } else {
    std::cerr << "Error getting receiver" << std::endl;
    return;
  }

  // Get subject and do the same string manipulation
  do {
    std::cout << "Subject (max. 80 chars): ";
    if (std::getline(std::cin, subject)) {
      subject.erase(0, subject.find_first_not_of(" \t\n\r\f\v"));
      subject.erase(subject.find_last_not_of(" \t\n\r\f\v") + 1);
    } else {
      std::cerr << "Error getting subject" << std::endl;
      return;
    }
    if (subject.length() > SUBJ)
      std::cerr << "Subject too long max. 80 chars\n";
  } while (subject.length() > SUBJ);

  // Get message
  std::cout << "Message, ends with a single dot on the next line:\n";
  while (true) {
    std::string temp;
    if (std::getline(std::cin, temp)) {
      if (temp == ".") {
        break; // End input
      }
      message += temp + "\n"; // Append to message with newline
    } else {
      std::cerr << "Error getting message" << std::endl;
      break;
    }
  }

  // Format Message
  std::string buffer =
      "SEND\n" + receiver + "\n" + subject + "\n" + message + ".\n";

  // Send message
  client->sendMessage(buffer);

  // Print Received response
  std::cout << client->receiveMessage();
}

void ClientHandler::handleList() {
  // Format message
  std::string buffer = "LIST\n";

  // Send message
  client->sendMessage(buffer);

  // Print Received response
  std::cout << client->receiveMessage();
}

void ClientHandler::handleRead() {
  std::string msg_nr = "";

  // Get message number and remove newline
  std::cout << "Message Number: ";
  if (std::getline(std::cin, msg_nr)) {
  } else {
    std::cerr << "Error reading message number" << std::endl;
    return;
  }

  // Format message
  std::string buffer = "READ\n" + msg_nr + '\n';

  // Send message
  client->sendMessage(buffer);

  // Print Received response
  std::cout << client->receiveMessage();
}

void ClientHandler::handleDelete() {
  std::string msg_nr = "";

  // Get message number and remove newline
  std::cout << "Message Number: ";
  if (std::getline(std::cin, msg_nr)) {
  } else {
    std::cerr << "Error reading message number" << std::endl;
    return;
  }

  // Format message safely with size limits
  std::string buffer = "DEL\n" + msg_nr + '\n';

  // Send message
  client->sendMessage(buffer);

  // Print Received response
  std::cout << client->receiveMessage();
}

int ClientHandler::getch() {
  int ch;
  // https://man7.org/linux/man-pages/man3/termios.3.html
  struct termios t_old, t_new;
  // https://man7.org/linux/man-pages/man3/termios.3.html
  // tcgetattr() gets the parameters associated with the object referred
  //   by fd and stores them in the termios structure referenced by termios_p
  tcgetattr(STDIN_FILENO, &t_old);
  // copy old to new to have a base for setting c_lflags
  t_new = t_old;
  // https://man7.org/linux/man-pages/man3/termios.3.html
  // ICANON Enable canonical mode (described below).
  //   * Input is made available line by line (max 4096 chars).
  //   * In noncanonical mode input is available immediately.
  // ECHO   Echo input characters.
  t_new.c_lflag &= ~(ICANON | ECHO);
  // sets the attributes
  // TCSANOW: the change occurs immediately.
  tcsetattr(STDIN_FILENO, TCSANOW, &t_new);
  ch = getchar();
  // reset stored attributes
  tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
  return ch;
}
const std::string ClientHandler::getpass() {
  int show_asterisk = 0;

  const char BACKSPACE = 127;
  const char RETURN = 10;

  unsigned char ch = 0;
  std::string password;

  while ((ch = getch()) != RETURN) {
    if (ch == BACKSPACE) {
      if (password.length() != 0) {
        if (show_asterisk) {
          std::cout << "\b \b"; // backslash: \b
        }
        password.resize(password.length() - 1);
      }
    } else {
      password += ch;
      if (show_asterisk) {
        std::cout << "*";
      }
    }
  }
  std::cout << std::endl;
  return password;
}
