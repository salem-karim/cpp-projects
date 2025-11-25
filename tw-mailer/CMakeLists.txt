cmake_minimum_required(VERSION 3.28.1)
project(tw_mailer)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set common compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O3 -pthread -mavx2 -g")

# Include the "include" directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Use pkg-config to find LDAP
find_package(PkgConfig REQUIRED)
pkg_check_modules(LDAP REQUIRED ldap)

# Include LDAP directories
include_directories(${LDAP_INCLUDE_DIRS})

# Define the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the server executable
add_executable(server
    ./server/server.cpp
    ./server/mailHandler.cpp
    ./server/server_socket.cpp
    ./server/server_handler.cpp
    ./server/authService.cpp
)

# Link the LDAP libraries to the server
target_link_libraries(server ${LDAP_LIBRARIES})

# Add the client executable
#add_executable(client
#    ./client/client.cpp
#    ./client/client_socket.cpp
#    ./client/client_handler.cpp
#)

# Link any required libraries to the client if needed (e.g., pthreads, etc.)
target_link_libraries(client pthread)
