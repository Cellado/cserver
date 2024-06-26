#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
// motor control
#include <pigpio.h>

#define MOTOR1F 17
#define MOTOR1B 27
#define MOTOR2F 5
#define MOTOR2B 6

#define IN1 23
#define IN2 24
#define IN3 25
#define IN4 16 


void init_motor() {

  if (gpioInitialise() < 0) {
    fprintf(stderr, "-1 initialization failed\n");
    exit(1); 
  }
  
  gpioSetMode(MOTOR1F, PI_OUTPUT);
  gpioSetMode(MOTOR1B, PI_OUTPUT);
  gpioSetMode(MOTOR2F, PI_OUTPUT);
  gpioSetMode(MOTOR2B, PI_OUTPUT);
  
}

void set_pins(int M1F, int M1B, int M2F, int M2B) {
  gpioWrite(MOTOR1F, M1F);
  gpioWrite(MOTOR1B, M1B);
  gpioWrite(MOTOR2F, M2F);
  gpioWrite(MOTOR2B, M2B);
}

int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
}; 

void setStep(int w1, int w2, int w3, int w4) {
  gpioWrite(IN1, w1);
  gpioWrite(IN2, w2);
  gpioWrite(IN3, w3);
  gpioWrite(IN4, w4);
}

void stepMotorF(int steps) {
  for (int i = 0; i < steps; ++i) {
    for (int j = 0; j < 8; ++j) {
      setStep(stepSequence[j][0], stepSequence[j][1], stepSequence[j][2], stepSequence[j][3]);
      usleep(1000);  // Adjust delay for speed control
    }
  }
}


void stepMotorB(int steps) {
  for (int i = 0; i < steps; ++i) {
    for (int j = 7; j >= 0; --j) {
      setStep(stepSequence[j][0], stepSequence[j][1], stepSequence[j][2], stepSequence[j][3]);
      usleep(1000);  // Adjust delay for speed control
    }
  }
}
  

/*
 * server program.
 */
int main(int argc, char*argv[]) {
  /* if (argc < 2) { */
  /*   printf("need port num\n"); */
  /*   exit(0); */
  /* } */
  
  init_motor();
  
  // Af_INET: ipv4
  // SOCK_STREAM: tcp
  // 0: system chooses tcp
  int server = socket(AF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    printf("error making socket\n");
    return(1);
  }
  
  // port being used
  //  int port = atoi(argv[1]);
  int port = 5138; 
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
  // 5 : server will queue 5 connections.
  if (listen(server, 5) < 0) {
    printf("listen failed\n");
    close(server);
    return(1);
  }

  while (true) {
    // store client address
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
  
    // accepts connection on server
    int newsock = accept(server, (struct sockaddr *)&client_address, &client_address_len);
    char client_ip[INET_ADDRSTRLEN];
  
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Connection accepted from %s:%d\n", client_ip, ntohs(client_address.sin_port));
    char buffer[1024] = {0};
    int read = recv(newsock, buffer, sizeof(buffer), 0);
  
    if(read > 0) {
      printf("The server recived: %s\n", buffer);
      buffer[read] = '\0'; // ensure null termination
    
      // convert to lower case
      for(int i = 0; buffer[i]; i++) {
	buffer[i] = tolower(buffer[i]); 
      }

      if(strcmp(buffer, "on") == 0) {
	set_pins(1, 0, 1, 0);
	stepMotorF(512); 
	printf("motors == 1\n"); 
      } else if (strcmp(buffer, "off") == 0) {
	set_pins(0, 0, 0, 0);
	stepMotorB(512); 
	printf("motors == 0\n");
      } else if (strcmp(buffer, "close") == 0) {
	close(newsock);
	close(server);
	exit(0); 
      } else {
	printf("invalid command"); 
      }
    
    } else if (read == 0) {
      printf("connection closed by client\n");
    } else {
      printf("error reading\n");
    }
    close(newsock);
  }
  close(server);
  return 0;
}

