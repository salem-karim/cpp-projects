#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> // For mkdir function
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdio> // For remove function

using namespace std;

///////////////////////////////////////////////////////////////////////////////

#define BUF 1024
#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

int abortRequested = 0;
int create_socket = -1;
int new_socket = -1;

///////////////////////////////////////////////////////////////////////////////

void *clientCommunication(void *data);
void signalHandler(int sig);
void splitIntoParts(const char *input, vector<string> &parts, int number_of_splits);
int send_to_client(int client_socket, const string &msg, const string error_msg);

///////////////////////////////////////////////////////////////////////////////

void splitIntoParts(const char *input, vector<string> &parts, int number_of_splits)
{
   size_t start = 0;
   string str_input(input); // Convert char array to string

   for (int i = 0; i < number_of_splits - 1; ++i)
   {
      size_t end = str_input.find('\n', start);

      // If end is not found, set it to the end of the string for the last part
      if (end == string::npos)
      {
         end = str_input.length();
      }

      // Add the part to the vector
      parts.push_back(str_input.substr(start, end - start));

      // Update start position for the next part
      start = end + 1; // Move past the newline character

      // If we reached the last newline and there are still more parts to add
      if (start >= str_input.length())
      {
         break; // Exit if we've reached the end of the string
      }
   }

   // If less parts are created than needed, we need to ensure the vector size matches
   while (static_cast<int>(parts.size()) < number_of_splits - 1)
   {
      parts.push_back(""); // Fill empty parts if needed
   }

   // Capture any remaining text as the last part if there's still text left
   if (start < str_input.length())
   {
      parts.push_back(str_input.substr(start));
   }
}

///////////////////////////////////////////////////////////////////////////////

int main(void)
{
   socklen_t addrlen;
   struct sockaddr_in address, cliaddress;
   int reuseValue = 1;

   ////////////////////////////////////////////////////////////////////////////
   // SIGNAL HANDLER
   // SIGINT (Interrupt: ctrl+c)
   if (signal(SIGINT, signalHandler) == SIG_ERR)
   {
      perror("signal can not be registered");
      return EXIT_FAILURE;
   }

   ////////////////////////////////////////////////////////////////////////////
   // CREATE A SOCKET
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("Socket error");
      return EXIT_FAILURE;
   }

   ////////////////////////////////////////////////////////////////////////////
   // SET SOCKET OPTIONS
   if (setsockopt(create_socket,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &reuseValue,
                  sizeof(reuseValue)) == -1)
   {
      perror("set socket options - reuseAddr");
      return EXIT_FAILURE;
   }

   if (setsockopt(create_socket,
                  SOL_SOCKET,
                  SO_REUSEPORT,
                  &reuseValue,
                  sizeof(reuseValue)) == -1)
   {
      perror("set socket options - reusePort");
      return EXIT_FAILURE;
   }

   ////////////////////////////////////////////////////////////////////////////
   // INIT ADDRESS
   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(PORT);

   ////////////////////////////////////////////////////////////////////////////
   // ASSIGN AN ADDRESS WITH PORT TO SOCKET
   if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
   {
      perror("bind error");
      return EXIT_FAILURE;
   }

   ////////////////////////////////////////////////////////////////////////////
   // ALLOW CONNECTION ESTABLISHING
   if (listen(create_socket, 5) == -1)
   {
      perror("listen error");
      return EXIT_FAILURE;
   }

   while (!abortRequested)
   {
      printf("Waiting for connections...\n");

      addrlen = sizeof(struct sockaddr_in);
      if ((new_socket = accept(create_socket,
                               (struct sockaddr *)&cliaddress,
                               &addrlen)) == -1)
      {
         if (abortRequested)
         {
            perror("accept error after aborted");
         }
         else
         {
            perror("accept error");
         }
         break;
      }

      printf("Client connected from %s:%d...\n",
             inet_ntoa(cliaddress.sin_addr),
             ntohs(cliaddress.sin_port));
      clientCommunication(&new_socket); // handle communication with client
      new_socket = -1;
   }

   // Cleanup
   if (create_socket != -1)
   {
      if (shutdown(create_socket, SHUT_RDWR) == -1)
      {
         perror("shutdown create_socket");
      }
      if (close(create_socket) == -1)
      {
         perror("close create_socket");
      }
      create_socket = -1;
   }

   return EXIT_SUCCESS;
}

void *clientCommunication(void *data)
{
   char buffer[BUF];
   int size;
   int *current_socket = (int *)data;

   ////////////////////////////////////////////////////////////////////////////
   // SEND welcome message
   string welcomeMsg = "Welcome to myserver!\r\nPlease enter your commands...\r\n";
   if (send_to_client(*current_socket, welcomeMsg, "") == -1)
      return NULL;

   do
   {
      /////////////////////////////////////////////////////////////////////////
      // RECEIVE
      size = recv(*current_socket, buffer, BUF - 1, 0);
      if (size == -1)
      {
         if (abortRequested)
         {
            perror("recv error after aborted");
         }
         else
         {
            perror("recv error");
         }
         break;
      }

      if (size == 0)
      {
         printf("Client closed remote socket\n"); // ignore error
         break;
      }

      // Remove any newline characters from the buffer
      if (size > 0 && buffer[size - 1] == '\n')
      {
         buffer[size - 1] = '\0';
         size--;
      }

      // Null-terminate the buffer
      buffer[size] = '\0';

      // Create a vector to hold request parts
      vector<string> request;
      splitIntoParts(buffer, request, 2); // Only the command

      if (!request.empty() && request[0] == "SEND")
      {
         vector<string> parts;
         splitIntoParts(buffer, parts, 5); // Split the buffer into parts

         // Specify the path where you want to create the file
         string directory = "./emails/" + parts[2]; // Change this to your desired folder
         filesystem::create_directories(directory); // Create directory if it doesn't exist

         // Count existing files to generate a new filename
         int fileCount = 0;
         try
         {
            for (const auto &entry : filesystem::directory_iterator(directory))
            {
               if (entry.is_regular_file())
               {
                  fileCount++;
               }
            }
         }
         catch (const filesystem::filesystem_error &e)
         {
            cerr << "Filesystem error: " << e.what() << endl;
            return NULL; // Exit with error code
         }

         // Construct the filename
         string filename = parts[2] + "_" + to_string(fileCount) + ".txt"; // Use to_string for integer
         string fullPath = directory + "/" + filename;

         // Create an output file stream
         ofstream outFile(fullPath);
         if (!outFile.is_open())
         {
            if (send_to_client(*current_socket, "ERR", "Error while opening file") == -1)
               return NULL;
         }

         // Write content to the file
         outFile << "Subject: " + parts[3] << endl;
         outFile << parts[4] << endl;

         // Close the file
         outFile.close();

         cout << "File created successfully: " << fullPath << endl;

         if (send_to_client(*current_socket, "OK", "") == -1)
            return NULL;
      }

      else if (!request.empty() && request[0] == "LIST")
      {
         string username = request[1]; // Get the username from the request
         vector<string> messageSubjects;

         string dir = "./emails/" + username;

         // Check if the directory exists before iterating
         if (!filesystem::exists(dir))
         {
            if (send_to_client(*current_socket, "0\n", "No messages found") == -1)
               return NULL; // Send a message count of 0
         }
         else
         {
            // Count the messages and store the subject lines
            for (const auto &entry : filesystem::directory_iterator(dir))
            {
               if (entry.is_regular_file())
               {
                  string filename = entry.path().filename().string();
                  // Check if the filename starts with the username
                  if (filename.find(username + "_") == 0)
                  {
                     // Extract the subject from the file
                     ifstream inFile(entry.path());
                     string subject;
                     if (getline(inFile, subject))
                     {
                        messageSubjects.push_back(subject); // Save the subject
                     }
                     inFile.close(); // Close the file after reading
                  }
               }
            }

            // Respond with the message count
            snprintf(buffer, BUF, "%zu\n", messageSubjects.size()); // Number of messages
            if (send_to_client(*current_socket, buffer, "") == -1)
               return NULL; // Send the count

            // Send the subjects
            if (messageSubjects.empty())
            {
               // Send a message indicating no subjects
               if (send_to_client(*current_socket, "\n", "") == -1)
                  return NULL;
            }
            else
            {
               for (const auto &subject : messageSubjects)
               {
                  snprintf(buffer, BUF, "%s\n", subject.c_str());
                  if (send_to_client(*current_socket, buffer, "") == -1)
                     return NULL;
               }
            }
         }
      }

      // READ command handling
      else if (!request.empty() && request[0] == "READ")
      {
         vector<string> parts;
         splitIntoParts(buffer, parts, 3); // Split the buffer into parts

         // Ensure that parts have enough elements
         if (parts.size() < 3)
         {
            if (send_to_client(*current_socket, "ERR", "Problem with splitting") == -1)
               return NULL;
         }

         string filename = "./emails/" + parts[1] + "/" + parts[1] + "_" + parts[2] + "txt";
         ifstream inputFile(filename); // Open the file

         // Check if the file opened successfully
         if (!inputFile.is_open())
         {
            if (send_to_client(*current_socket, "ERR", "Error while opening File (READ)") == -1)
               return NULL;
         }

         string line;
         string msg;

         // Read and ignore the first line (subject)
         if (getline(inputFile, line))
         {
            // This line is ignored
         }

         msg += "OK\n";
         // Read the rest of the file and accumulate the content
         while (getline(inputFile, line))
         {
            msg += line + "\n"; // Append each line to msg
         }

         // Close the file
         inputFile.close();

         if (send_to_client(*current_socket, msg, "") == -1)
            return NULL;
      }

      else if (!request.empty() && request[0] == "DEL")
      {
         vector<string> parts;
         splitIntoParts(buffer, parts, 3); // Split the buffer into parts

         // Ensure that parts have enough elements
         if (parts.size() < 3)
         {
            if (send_to_client(*current_socket, "ERR", "Problem with splitting") == -1)
               return NULL;
         }

         string filename = "./emails/" + parts[1] + "/" + parts[1] + "_" + parts[2] + "txt";

         if (remove(filename.c_str()) == 0)
         {
            if (send_to_client(*current_socket, "OK", "") == -1)
               return NULL;
         }
         else
         {
            if (send_to_client(*current_socket, "ERR", "Problem while deleting File") == -1)
               return NULL;
         }
      }

   } while (strcmp(buffer, "quit") != 0 && !abortRequested);

   // closes/frees the descriptor if not already
   if (*current_socket != -1)
   {
      if (shutdown(*current_socket, SHUT_RDWR) == -1)
      {
         perror("shutdown new_socket");
      }
      if (close(*current_socket) == -1)
      {
         perror("close new_socket");
      }
      *current_socket = -1;
   }

   return NULL;
}

void signalHandler(int sig)
{
   if (sig == SIGINT)
   {
      printf("abort Requested... "); // ignore error
      abortRequested = 1;

      // Shutdown and close the sockets
      if (new_socket != -1)
      {
         if (shutdown(new_socket, SHUT_RDWR) == -1)
         {
            perror("shutdown new_socket");
         }
         if (close(new_socket) == -1)
         {
            perror("close new_socket");
         }
         new_socket = -1;
      }

      if (create_socket != -1)
      {
         if (shutdown(create_socket, SHUT_RDWR) == -1)
         {
            perror("shutdown create_socket");
         }
         if (close(create_socket) == -1)
         {
            perror("close create_socket");
         }
         create_socket = -1;
      }
   }
   else
   {
      exit(sig);
   }
}

int send_to_client(int client_socket, const string &msg, const string error_msg)
{
   if (msg == "ERR")
   {
      send(client_socket, "ERR", 3, 0);
      cout << error_msg << endl;
      return -1;
   }

   if (send(client_socket, msg.c_str(), msg.length(), 0) == -1)
   {
      perror("send error");
      send(client_socket, "ERR", 3, 0);
      return -1;
   }
   return 0;
}
