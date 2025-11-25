#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF 1024
#define PORT 6543

int main(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;
    int isQuit = 0;

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
    if (connect(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

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
                char sender[50], receiver[50], subject[100], message[BUF] = "";

                // Get sender
                printf("Sender: ");
                if (fgets(sender, sizeof(sender), stdin) != NULL) {
                    sender[strcspn(sender, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading sender");
                    continue; // Handle error by continuing to the next iteration
                }

                // Get receiver
                printf("Receiver: ");
                if (fgets(receiver, sizeof(receiver), stdin) != NULL) {
                    receiver[strcspn(receiver, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading receiver");
                    continue; // Handle error by continuing to the next iteration
                }

                // Get subject
                printf("Subject: ");
                if (fgets(subject, sizeof(subject), stdin) != NULL) {
                    subject[strcspn(subject, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading subject");
                    continue; // Handle error by continuing to the next iteration
                }

                // Get message
                printf("Message (end with a single dot on a new line):\n");
                while (1) {
                    char temp[BUF];
                    if (fgets(temp, BUF, stdin) != NULL) {
                        if (strcmp(temp, ".\n") == 0) {
                            break; // End input
                        }
                        strcat(message, temp); // Append to message
                    } else {
                        perror("Error reading message");
                        break; // Handle error by breaking the loop
                    }
                }

                // Format message
                int ret = snprintf(buffer, BUF, "SEND\n%s\n%s\n%s\n%s.\n", sender, receiver, subject, message);
                if (ret < 0 || ret >= BUF) {
                    fprintf(stderr, "Error: message too long to send.\n");
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
                char username[50];
                printf("Username: ");
                if (fgets(username, sizeof(username), stdin) != NULL) {
                    username[strcspn(username, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading username");
                    continue; // Handle error by continuing to the next iteration
                }

                // Format message
                int ret = snprintf(buffer, BUF, "LIST\n%s\n", username);
                if (ret < 0 || ret >= BUF) {
                    fprintf(stderr, "Error: message too long to send.\n");
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
                char username[50], msg_nr[10];
                printf("Username: ");
                if (fgets(username, sizeof(username), stdin) != NULL) {
                    username[strcspn(username, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading username");
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
                    fprintf(stderr, "Error: message too long to send.\n");
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
                char username[50], msg_nr[10];
                printf("Username: ");
                if (fgets(username, sizeof(username), stdin) != NULL) {
                    username[strcspn(username, "\n")] = '\0'; // Remove newline
                } else {
                    perror("Error reading username");
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
                int ret = snprintf(buffer, BUF, "DEL\n%s\n%s.\n", username, msg_nr);
                if (ret < 0 || ret >= BUF) {
                    fprintf(stderr, "Error: message too long to send.\n");
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
            perror("Error reading input");
            isQuit = 1; // Exit loop on error
        }
    } while (!isQuit);

    close(create_socket);
    return 0;
}
