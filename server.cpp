#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sstream>

void handle_post_request(int client_fd, const char* request) {
   std::string request_str(request);
   size_t body_start = request_str.find("\r\n\r\n");
   std::string body = request_str.substr(body_start + 4);

   std::string name;
   size_t name_pos = body.find("name=");
   if (name_pos != std::string::npos) {
      name = body.substr(name_pos + 5);
   }

   std::string response_body = "Hello, " + name;
   
   const std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
        "\r\n" +
        response_body;

   write(client_fd, response.c_str(), response.size());     
}

int main() {
   int server_fd, client_fd;
   struct sockaddr_in server_addr;
   char buffer[1024] = {0};

   server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd == -1) {
      std::cerr << "Error creating socket\n"<<std::endl;
      return 1;
   }

   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(8080);

   if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      std::cerr << "Socket binding error\n";
      return 1;
   }

   if (listen(server_fd, 3) < 0) {
      std::cerr << "Error while listening\n";
      return 1;
   }

   std::cout << "The server is running on port 8080...\n";

   while((client_fd = accept(server_fd, nullptr, nullptr)) >= 0) {
      read(client_fd, buffer, 1024);
      std::cout << "Request received:\n" << buffer << std::endl;

      if (strncmp(buffer, "POST", 4) == 0) {
         handle_post_request(client_fd, buffer);
      } else {
         const char* response =
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 19\r\n"
                "\r\n"
                "405 Method Not Allowed";

         write(client_fd, response, strlen(response));
      }
      close(client_fd);      
   }

   close(server_fd);
   return 0;
}

