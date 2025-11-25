#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////

#define BUF 1024
#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

int abortRequested = 0;
int server = -1;
int client = -1;

///////////////////////////////////////////////////////////////////////////////

void *clientCommunication(void *data);
void signalHandler(int sig);

///////////////////////////////////////////////////////////////////////////////

int main(void) {
  socklen_t addrlen;
  struct sockaddr_in address, cliaddress;
  int reuseValue = 1;

  ////////////////////////////////////////////////////////////////////////////
  // SIGNAL HANDLER
  // SIGINT (Interrup: ctrl+c)
  // https://man7.org/linux/man-pages/man2/signal.2.html
  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    perror("signal can not be registered");
    return EXIT_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////
  // CREATE A SOCKET
  // https://man7.org/linux/man-pages/man2/socket.2.html
  // https://man7.org/linux/man-pages/man7/ip.7.html
  // https://man7.org/linux/man-pages/man7/tcp.7.html
  // IPv4, TCP (connection oriented), IP (same as client)
  if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket error"); // errno set by socket()
    return EXIT_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////
  // SET SOCKET OPTIONS
  // https://man7.org/linux/man-pages/man2/setsockopt.2.html
  // https://man7.org/linux/man-pages/man7/socket.7.html
  // socket, level, optname, optvalue, optlen
  if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuseValue,
                 sizeof(reuseValue)) == -1) {
    perror("set socket options - reuseAddr");
    return EXIT_FAILURE;
  }

  if (setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &reuseValue,
                 sizeof(reuseValue)) == -1) {
    perror("set socket options - reusePort");
    return EXIT_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////
  // INIT ADDRESS
  // Attention: network byte order => big endian
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  ////////////////////////////////////////////////////////////////////////////
  // ASSIGN AN ADDRESS WITH PORT TO SOCKET
  if (bind(server, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("bind error");
    return EXIT_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////
  // ALLOW CONNECTION ESTABLISHING
  // Socket, Backlog (= count of waiting connections allowed)
  if (listen(server, 5) == -1) {
    perror("listen error");
    return EXIT_FAILURE;
  }

  while (!abortRequested) {
    /////////////////////////////////////////////////////////////////////////
    // ignore errors here... because only information message
    // https://linux.die.net/man/3/printf
    printf("Waiting for connections...\n");

    /////////////////////////////////////////////////////////////////////////
    // ACCEPTS CONNECTION SETUP
    // blocking, might have an accept-error on ctrl+c
    addrlen = sizeof(struct sockaddr_in);
    if ((client = accept(server, (struct sockaddr *)&cliaddress, &addrlen)) ==
        -1) {
      if (abortRequested) {
        perror("accept error after aborted");
      } else {
        perror("accept error");
      }
      break;
    }

    /////////////////////////////////////////////////////////////////////////
    // START CLIENT
    // ignore printf error handling
    printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr),
           ntohs(cliaddress.sin_port));
    clientCommunication(&client); // returnValue can be ignored
    client = -1;
  }

  // frees the descriptor
  if (server != -1) {
    if (shutdown(server, SHUT_RDWR) == -1) {
      perror("shutdown server");
    }
    if (close(server) == -1) {
      perror("close server");
    }
    server = -1;
  }

  return EXIT_SUCCESS;
}

void *clientCommunication(void *data) {
  char buffer[BUF];
  int size;
  int *current_client = (int *)data;

  ////////////////////////////////////////////////////////////////////////////
  // SEND welcome message
  strcpy(buffer, "Welcome to myserver!\r\nPlease enter your commands...\r\n");
  if (send(*current_client, buffer, strlen(buffer), 0) == -1) {
    perror("send failed");
    return NULL;
  }

  do {
    /////////////////////////////////////////////////////////////////////////
    // RECEIVE
    size = recv(*current_client, buffer, BUF - 1, 0);
    if (size == -1) {
      if (abortRequested) {
        perror("recv error after aborted");
      } else {
        perror("recv error");
      }
      break;
    }

    if (size == 0) {
      printf("Client closed remote socket\n"); // ignore error
      break;
    }

    // remove ugly debug message, because of the sent newline of client
    if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
      size -= 2;
    } else if (buffer[size - 1] == '\n') {
      --size;
    }

    buffer[size] = '\0';
    // Here if blocks like if (strcmp(buffer, "list") == 0) oder "LIST"
    printf("Message received: %s\n", buffer); // ignore error

    if (send(*current_client, "OK", 3, 0) == -1) {
      perror("send answer failed");
      return NULL;
    }
  } while (strcmp(buffer, "quit") != 0 && !abortRequested);

  // closes/frees the descriptor if not already
  if (*current_client != -1) {
    if (shutdown(*current_client, SHUT_RDWR) == -1) {
      perror("shutdown client");
    }
    if (close(*current_client) == -1) {
      perror("close client");
    }
    *current_client = -1;
  }

  return NULL;
}

void signalHandler(int sig) {
  if (sig == SIGINT) {
    printf("abort Requested... "); // ignore error
    abortRequested = 1;
    /////////////////////////////////////////////////////////////////////////
    // With shutdown() one can initiate normal TCP close sequence ignoring
    // the reference count.
    // https://beej.us/guide/bgnet/html/#close-and-shutdownget-outta-my-face
    // https://linux.die.net/man/3/shutdown
    if (client != -1) {
      if (shutdown(client, SHUT_RDWR) == -1) {
        perror("shutdown client");
      }
      if (close(client) == -1) {
        perror("close client");
      }
      client = -1;
    }

    if (server != -1) {
      if (shutdown(server, SHUT_RDWR) == -1) {
        perror("shutdown server");
      }
      if (close(server) == -1) {
        perror("close server");
      }
      server = -1;
    }
  } else {
    exit(sig);
  }
}
