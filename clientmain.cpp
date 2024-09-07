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

  if (strstr(buf, protocol) != NULL) {
    return true;
  }
  return false;
}

bool send_message(int clientSocket, const char* message) {
    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent < 0) {
        return false;
    } else {
        return true;
    }
}
char* do_magic(char* buf){

  char delim[]=" ";
  char *operation = strtok(buf, delim); 
  char *value1_str= strtok(NULL, delim);
  char *value2_str = strtok(NULL, "\n");
  int result;
  double result_double;
  static char result_str[20];
  char* float_format = "%8.8g";
  char* int_format = "%d";

  if (strstr(buf, "fdiv") != NULL) {
    double value1 = atof(value1_str);
    double value2 = atof(value2_str);
    result_double = value1 / value2;
    sprintf(result_str, float_format, result_double);
    }
  else   if (strstr(buf, "fadd") != NULL) {
    double value1 = atof(value1_str);
    double value2 = atof(value2_str);
    result_double = value1 + value2;
    sprintf(result_str, float_format, result_double);
    }
  else   if (strstr(buf, "fmul") != NULL) {
    double value1 = atof(value1_str);
    double value2 = atof(value2_str);
    result_double = value1 * value2;
    sprintf(result_str, float_format, result_double);
    }
  else   if (strstr(buf, "fsub") != NULL) {
    double value1 = atof(value1_str);
    double value2 = atof(value2_str);
    result_double= value1 - value2;
    sprintf(result_str, float_format, result_double);
    }
  else   if (strstr(buf, "fdiv") != NULL) {
    double value1 = atof(value1_str);
    double value2 = atof(value2_str);
    result_double = value1 / value2;
    sprintf(result_str, float_format, result_double);
    }
  else   if (strstr(buf, "add") != NULL) {
    int value1 = atoi(value1_str);
    int value2 = atoi(value2_str);
    result = value1 + value2;
    sprintf(result_str, int_format, result);
    }
  else   if (strstr(buf, "div") != NULL) {
    int value1 = atoi(value1_str);
    int value2 = atoi(value2_str);
    result = value1 / value2;
    sprintf(result_str, int_format, result);
    }
  else   if (strstr(buf, "mul") != NULL) {
    int value1 = atoi(value1_str);
    int value2 = atoi(value2_str);
    result = value1 * value2;
    sprintf(result_str, int_format, result);
    }
  else   if (strstr(buf, "sub") != NULL) {
    int value1 = atoi(value1_str);
    int value2 = atoi(value2_str);
    result = value1 - value2;
    sprintf(result_str, int_format, result);
    }
  return result_str;
}

int main(int argc, char *argv[]){
  
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 





  int port=atoi(Destport);


  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress; 
  serverAddress.sin_family = AF_INET; 
  serverAddress.sin_port = htons(port); 
  inet_pton(AF_INET, Desthost, &serverAddress.sin_addr); 
  if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
      printf("ERROR: Cannot connect to %s:%d.\n",Desthost,port);
      return -1;
  }


  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
  if(check_protocol(buffer)){
    send_message(clientSocket, "OK\n");
  }
  else{
    printf("ERROR: MISSMATCH PROTOCOL\n");
    close(clientSocket);
  }


  memset(buffer, 0, sizeof(buffer));
  recv(clientSocket, buffer, sizeof(buffer), 0);

  
  char* result = do_magic(buffer);
  send_message(clientSocket, result);
  

#ifdef DEBUG 
  printf("Hhet %s, and port %d.\n",Desthost,port);
#endif
  close(clientSocket);
  return 0;
}
