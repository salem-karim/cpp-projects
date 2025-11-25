#include <cassert> // for assert
#include <cstdlib> // for EXIT_SUCCESS
#include <cstring> // for strcpy, strlen
#include <iostream>
#include <sys/ipc.h> // for IPC_CREAT, IPC_RMID
#include <sys/msg.h> // for msgget, msgsnd, msgrcv
#include <sys/wait.h> // for waitpid
#include <unistd.h> // for getopt, fork, getpid
#include "search.h"

using std::string, std::vector, std::endl, std::cerr;

void print_usage(const char *program_name) {
  cerr << "Usage: " << program_name << " [-R] [-i] searchpath filename1 [filename2] [filenameN]" << endl;
}

struct message {
  long msg_type;
  char text[256];
};

int main(int argc, char *argv[]) {
  int opts = 0;
  int error = 0;
  bool recursive = false;
  bool case_insensitive = false;

  const char *searchPath = nullptr;
  const char *filenames[argc]; // Store the filenames
  int file_count = 0;

  const char *program_name = argv[0]; // Store program name
  opterr = 0;

  // First, loop through arguments to identify search path and options
  while ((opts = getopt(argc, argv, "Ri")) != -1) {
    switch (opts) {
      case 'R': // Recursive option
        if (recursive) { // Check if -R has been used before
          cerr << program_name << ": option -R used more than once" << endl;
          error = 1;
          break;
        }
        recursive = true;
        break;
      case 'i': // Ignore case option
        if (case_insensitive) { // Check if -i has been used before
          cerr << program_name << ": option -i used more than once" << endl;
          error = 1;
          break;
        }
        case_insensitive = true;
        break;
      case '?': // Invalid option
        error = 1;
        break;
      default: // Impossible case
        assert(0);
    }
  }

  if (error) { // Invalid options or options used multiple times
    print_usage(program_name);
    return EXIT_FAILURE;
  }

  // After options are processed, optind points to the first positional argument
  if (optind < argc) {
    searchPath = argv[optind]; // The first non-option argument is the search path
    optind++; // Move to the next argument
  } else {
    print_usage(program_name);
    return EXIT_FAILURE;
  }

  // Collect the remaining filenames (filename1, filename2, ..., filenameN)
  while (optind < argc) {
    filenames[file_count++] = argv[optind++];
  }

  // If no filenames were given, print usage and exit
  if (file_count == 0) {
    print_usage(program_name);
    return EXIT_FAILURE;
  }

  // Create a message queue
  int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
  if (msgid == -1) {
    cerr << "Failed to create message queue" << endl;
    return EXIT_FAILURE;
  }

  for (int i = 0; i < file_count; ++i) {
    pid_t pid = fork();
    if (pid == -1) {
      cerr << "Fork failed!" << endl;
      return EXIT_FAILURE;
    } else if (pid == 0) {
      // Child process
      vector<string> results;
      results.reserve(file_count);
      findFile(results, searchPath, filenames[i], recursive, case_insensitive);
      message msg = {1,{}};
      msg.msg_type = 1;
      // format msg.text with Information and send the message to the queue
      if (!results.empty()) {
        // If there is a result iterate through the vector and give the message the result path
        for (const auto &result: results) {
          snprintf(msg.text, sizeof(msg.text), "%d: %s: %s\n", getpid(), filenames[i], result.c_str());
          msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);
        }
      } else {
        // Else just format it without the path
        snprintf(msg.text, sizeof(msg.text), "%d: %s: File not found\n", getpid(), filenames[i]);
        msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);
      }
      exit(EXIT_SUCCESS);
    }
  }

  // Parent process waits for the child processes to be finished
  for (int i = 0; i < file_count; ++i) {
    int status;
    wait(&status);
  }

  // Receive messages from the message queue and print it to stdout
  message msg = {0, {}};
  while (msgrcv(msgid, &msg, sizeof(msg.text), 0, IPC_NOWAIT) != -1) {
    std::cout << msg.text;
  }

  // Remove the message queue
  msgctl(msgid, IPC_RMID, nullptr);

  return EXIT_SUCCESS;
}
