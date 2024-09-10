#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
/* You will to add includes here */

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG


// Included to get the support library
#include <calcLib.h>



bool check_protocol(char* buf){
  const char* protocol = "TEXT TCP 1.0";
  bool flag = false;

  char* sub_buf = strtok(buf, "\n");

  while (sub_buf != NULL){
    if (strcmp(sub_buf, protocol) == 0){
      flag = true;
    }
    sub_buf = strtok(NULL,"\n");
  }

  return flag;
}

bool send_message(int clientSocket, const char* message) {
    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent < 0) {
        return false;
    } else {
        return true;
    }
}

float handle_float(char* op, char* v1, char* v2) {
    float num1 = atof(v1);
    float num2 = atof(v2);
    float result;

    if (strcmp(op, "fadd") == 0) {
        result = num1 + num2;

    } else if (strcmp(op, "fsub") == 0) {
        result = num1 - num2;

    } else if (strcmp(op, "fmul") == 0) {
        result = num1 * num2;

    } else if (strcmp(op, "fdiv") == 0) {
        result = num1 / num2;
    }
   return result;
}

int handle_int(char* op, char* v1, char* v2) {
    int num1 = atoi(v1);
    int num2 = atoi(v2);
    int result;

    if (strcmp(op, "add") == 0) {
        result = num1 + num2;

    } else if (strcmp(op, "sub") == 0) {
        result = num1 - num2;

    } else if (strcmp(op, "mul") == 0) {
        result = num1 * num2;

    } else if (strcmp(op, "div") == 0) {
        result = num1 / num2;
    }
return result;
}


void remove_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


int main(int argc, char *argv[]){

  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  int port=atoi(Destport);

  //create socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    printf("ERROR: Socket creation failed.\n");
    return -1;
}
  sockaddr_in serverAddress; 
  serverAddress.sin_family = AF_INET; 
  serverAddress.sin_port = htons(port); 
  inet_pton(AF_INET, Desthost, &serverAddress.sin_addr); 

  //connect to server
  if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
      printf("ERROR: CANT CONNECT TO  %s:%d.\n",Desthost,port);
      return -1;
  }

  //receive and check protocol
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  recv(clientSocket, buffer, sizeof(buffer), 0);
  if(check_protocol(buffer)){
    // send ok
    send_message(clientSocket,  "OK\n");
    printf("OK\n");
  }
  else{
    printf("ERROR: MISSMATCH PROTOCOL\n");
    close(clientSocket);
    return 0;
  }

  // receive operation
  memset(buffer, 0, sizeof(buffer));
  recv(clientSocket, buffer, sizeof(buffer), 0);

  // parse command
  char *op = strtok(buffer, " "); 
  char *v1= strtok(NULL, " ");
  char *v2 = strtok(NULL, "\n");
  printf("%s %s %s\n", op,v1,v2);
  char result_str[20];

  //  do work
  if (op[0] == 'f') {
    float result = handle_float(op,v1,v2);
    sprintf(result_str,"%8.8g\n",result);

  } else {
    int result = handle_int(op,v1,v2);
    sprintf(result_str,"%d\n",result);
  }

  //send back results
  send_message(clientSocket, result_str);

  // get response
  memset(buffer, 0, sizeof(buffer));
  recv(clientSocket, buffer, sizeof(buffer), 0);

  remove_newline(buffer);
  remove_newline(result_str);
  printf("%s (myresult=%s)\n",buffer,result_str);
  
#ifdef DEBUG 
  printf("Host %s, and port %d.\n",Desthost,port);
#endif
  close(clientSocket);
  return 0;
}
