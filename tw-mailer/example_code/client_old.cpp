#include <arpa/inet.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF 1024

// Function to check if a string is a valid integer
bool is_valid_port(const char *str) {
  for (int i = 0; str[i] != '\0'; ++i) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  int port = atoi(str);
  return port > 0 && port <= 65535; // Valid port range is 1-65535
}

int main(int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;
  int isQuit = 0;
  int PORT;

  // Check if IP and PORT are provided as arguments
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Validate the IP address
  if (inet_pton(AF_INET, argv[1], &(address.sin_addr)) != 1) {
    fprintf(stderr, "Error: Invalid IP address format.\n");
    return EXIT_FAILURE;
  }

  // Validate the port
  if (!is_valid_port(argv[2])) {
    fprintf(stderr, "Error: Invalid port number. Must be an integer between 1 "
                    "and 65535.\n");
    return EXIT_FAILURE;
  } else {
    PORT = atoi(argv[2]);
  }

  // CREATE A SOCKET
  if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket error");
    return EXIT_FAILURE;
  }

  // INIT ADDRESS
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  if (argc < 2) {
    inet_aton("127.0.0.1", &address.sin_addr);
  } else {
    inet_aton(argv[1], &address.sin_addr);
  }

  // CREATE A CONNECTION
  if (connect(create_socket, (struct sockaddr *)&address, sizeof(address)) ==
      -1) {
    perror("Connect error - no server available");
    return EXIT_FAILURE;
  }

  printf("Connection with server (%s) established\n",
         inet_ntoa(address.sin_addr));

  // RECEIVE DATA
  size = recv(create_socket, buffer, BUF - 1, 0);
  if (size == -1) {
    perror("recv error");
  } else if (size == 0) {
    printf("Server closed remote socket\n");
  } else {
    buffer[size] = '\0';
    printf("%s", buffer);
  }

  do {
    printf(">> ");
    // Check the return value of fgets
    if (fgets(buffer, BUF - 1, stdin) != NULL) {
      buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
      isQuit = strcmp(buffer, "quit") == 0;

      // SEND COMMAND
      if (strcmp(buffer, "SEND") == 0) {
        char sender[10], receiver[10], subject[80], message[BUF] = "";

        // Get sender
        printf("Sender: ");
        if (fgets(sender, sizeof(sender), stdin) != NULL) {
          sender[strcspn(sender, "\n")] = '\0'; // Strip newline character
        } else {
          perror("Error getting Sender");
          continue; // Handle the error by continuing to the next iteration
        }

        // Get receiver
        printf("Receiver: ");
        if (fgets(receiver, sizeof(receiver), stdin) != NULL) {
          receiver[strcspn(receiver, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error getting receiver");
          continue; // Handle error by continuing to the next iteration
        }

        // Get subject
        printf("Subject: ");
        if (fgets(subject, sizeof(subject), stdin) != NULL) {
          subject[strcspn(subject, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error getting subject");
          continue; // Handle error by continuing to the next iteration
        }

        // Get message
        printf("Message, ends with a single dont on the next line:\n");
        while (1) {
          char temp[BUF];
          if (fgets(temp, BUF, stdin) != NULL) {
            if (strcmp(temp, ".\n") == 0) {
              break; // End input
            }
            strcat(message, temp); // Append to message
          } else {
            perror("Error getting message");
            break; // Handle error by breaking the loop
          }
        }

        // Format message
        int ret = snprintf(buffer, BUF, "SEND\n%s\n%s\n%s\n%s.\n", sender,
                           receiver, subject, message);
        if (ret < 0 || ret >= BUF) {
          fprintf(stderr, "Error: This message is too long to send.\n");
          continue;
        }

        // Send message
        if (send(create_socket, buffer, strlen(buffer), 0) == -1) {
          perror("send error");
          continue;
        }

        // Receive response
        size = recv(create_socket, buffer, BUF - 1, 0);
        if (size > 0) {
          buffer[size] = '\0';
          printf("<< %s\n", buffer);
        }
        continue;
      }

      // LIST COMMAND
      if (strcmp(buffer, "LIST") == 0) {
        char username[10];
        printf("Username: ");
        if (fgets(username, sizeof(username), stdin) != NULL) {
          username[strcspn(username, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error getting username");
          continue; // Handle error by continuing to the next iteration
        }

        // Format message
        int ret = snprintf(buffer, BUF, "LIST\n%s\n", username);
        if (ret < 0 || ret >= BUF) {
          fprintf(stderr, "Error: This message is too long to send.\n");
          continue;
        }

        // Send message
        if (send(create_socket, buffer, strlen(buffer), 0) == -1) {
          perror("send error");
          continue;
        }

        // Receive response
        size = recv(create_socket, buffer, BUF - 1, 0);
        if (size > 0) {
          buffer[size] = '\0';
          printf("<< %s\n", buffer);
        }
        continue;
      }

      // READ COMMAND
      if (strcmp(buffer, "READ") == 0) {
        char username[10], msg_nr[10];
        printf("Username: ");
        if (fgets(username, sizeof(username), stdin) != NULL) {
          username[strcspn(username, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error getting username");
          continue; // Handle error by continuing to the next iteration
        }

        printf("Message Number: ");
        if (fgets(msg_nr, sizeof(msg_nr), stdin) != NULL) {
          msg_nr[strcspn(msg_nr, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error reading message number");
          continue; // Handle error by continuing to the next iteration
        }

        // Format message
        int ret = snprintf(buffer, BUF, "READ\n%s\n%s.\n", username, msg_nr);
        if (ret < 0 || ret >= BUF) {
          fprintf(stderr, "Error: This message too long to send.\n");
          continue;
        }

        // Send message
        if (send(create_socket, buffer, strlen(buffer), 0) == -1) {
          perror("send error");
          continue;
        }

        // Receive response
        size = recv(create_socket, buffer, BUF - 1, 0);
        if (size > 0) {
          buffer[size] = '\0';
          printf("<< %s\n", buffer);
        }
        continue;
      }

      // DEL COMMAND
      if (strcmp(buffer, "DEL") == 0) {
        char username[10], msg_nr[10];
        printf("Username: ");
        if (fgets(username, sizeof(username), stdin) != NULL) {
          username[strcspn(username, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error gettin username");
          continue; // Handle error by continuing to the next iteration
        }

        printf("Message Number: ");
        if (fgets(msg_nr, sizeof(msg_nr), stdin) != NULL) {
          msg_nr[strcspn(msg_nr, "\n")] = '\0'; // Remove newline
        } else {
          perror("Error getting message number");
          continue; // Handle error by continuing to the next iteration
        }

        // Format message
        int ret = snprintf(buffer, BUF, "DEL\n%s\n%s.\n", username, msg_nr);
        if (ret < 0 || ret >= BUF) {
          fprintf(stderr, "Error: This message too long to send.\n");
          continue;
        }

        // Send message
        if (send(create_socket, buffer, strlen(buffer), 0) == -1) {
          perror("send error");
          continue;
        }

        // Receive response
        size = recv(create_socket, buffer, BUF - 1, 0);
        if (size > 0) {
          buffer[size] = '\0';
          printf("<< %s\n", buffer);
        }
        continue;
      }

      // QUIT COMMAND
      if (strcmp(buffer, "QUIT") == 0) {
        isQuit = 1; // Set flag to exit loop
        continue;
      }

      // Other commands can be handled here

      // Send unknown command
      if (send(create_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send error");
        continue;
      }

      // Receive response
      size = recv(create_socket, buffer, BUF - 1, 0);
      if (size > 0) {
        buffer[size] = '\0';
        printf("<< %s\n", buffer);
      }
    } else {
      // Handle error with fgets
      perror("Error getting input");
      isQuit = 1; // Exit loop on error
    }
  } while (!isQuit);

  close(create_socket);
  return 0;
}
