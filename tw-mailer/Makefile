# Variables
CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -O3 -std=c++17 -pthread -mavx2 -g -I./include
LIBS = -lldap -llber

# Output Directories
BIN_DIR = bin

# Source Files
SERVER_SRC = ./server/server.cpp ./server/mailHandler.cpp ./server/server_socket.cpp ./server/server_handler.cpp ./server/authService.cpp
CLIENT_SRC = ./client/client.cpp ./client/client_socket.cpp ./client/client_handler.cpp

# Output Files
SERVER_BIN = $(BIN_DIR)/server
CLIENT_BIN = $(BIN_DIR)/client

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Create bin directory if it doesn't exist and compile server
$(SERVER_BIN): $(SERVER_SRC) | $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) -o $@ $^ ${LIBS}

# Create bin directory if it doesn't exist and compile client
$(CLIENT_BIN): $(CLIENT_SRC) | $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) -o $@ $^

# Create bin directory target
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean target
clean:
	$(RM) -r $(BIN_DIR)

.PHONY: all clean

# nix: CXXFLAGS += $(shell pkg-config --cflags ldap)
nix: CXX_FLAGS := $(shell pkg-config --cflags ldap) -I./include -Wall -Wextra -Werror -O3 -std=c++17 -pthread -mavx2
nix: LIBS := $(shell pkg-config --libs ldap)
nix: all
