//
// Created by sera on 26.09.24.
//
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    } else if (pid == 0) {
        // Child process
        std::cout << "Hello from the child process! PID: " << getpid() << std::endl;
    } else {
        // Parent process
        std::cout << "Hello from the parent process! PID: " << getpid() << " Child PID: " << pid << std::endl;

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}