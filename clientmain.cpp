#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/* You will to add includes here */

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG


// Included to get the support library
#include <calcLib.h>

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



  /* Do magic */

  int port=atoi(Destport);


  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress; 
  serverAddress.sin_family = AF_INET; 
  serverAddress.sin_port = htons(port); 
  inet_pton(AF_INET, Desthost, &serverAddress.sin_addr); 
  int connectResult = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

#ifdef DEBUG 
  printf("Hhet %s, and port %d.\n",Desthost,port);
#endif
  close(clientSocket);
  return connectResult;
}
