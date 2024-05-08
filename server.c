#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
/*
 * server program.
 */
int main(int argc, char*argv[]) {
  if (argc < 2) {
    printf("need port num");
    exit(0);
  }
  // Af_INET: ipv4
  // SOCK_STREAM: tcp
  // 0: system chooses tcp
  int server = socket(AF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    printf("error making socket\n");
    return(1);
  }
  // port being used
  int port = atoi(argv[1]);
  struct sockaddr_in address = {
    .sin_family = AF_INET, // ipv4 being used
    .sin_addr.s_addr = INADDR_ANY, // server will accept any connection.
    .sin_port = htons(port) // port byte order corrected.
  };
  // assigns address to socket
  // (struct sockaddr *)&address: bind needs: pointer to
  // generic sockaddr struct
  if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printf(" bind failed \n");
    close(server);
    return(1);
  }
  // prepares server to accept connections
  // 3 : server will queue 3 connections.
  if (listen(server, 3) < 0) {
    printf("listen failed\n");
    close(server);
    return(1);
  }


  // store client address
  struct sockaddr_in client_address;
  socklen_t client_address_len = sizeof(client_address);
  // accepts connection on server
  // NULL doesnt store clients address info
  int newsock = accept(server, (struct sockaddr *)&client_address, &client_address_len);
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
  printf("Connection accepted from %s:%d\n", client_ip, ntohs(client_address.sin_port));
  char buffer[1024] = {0};
  int read = recv(newsock, buffer, sizeof(buffer), 0);
  if(read > 0) {
    printf("The server recived: %s\n", buffer);
  } else {
    printf("error reading\n");
  }
  close(newsock);
  close(server);
  return 0;
}




































	
