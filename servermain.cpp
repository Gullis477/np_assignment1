#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
/* You will to add includes here */


// Included to get the support library
#include  "calcLib.h"

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG


using namespace std;


int get_operation(char* buf){
  /* Declarations, on top */
  double fv1,fv2,fresult;
  int iv1,iv2,iresult;


  /* within request loop */
  char *op=randomType();



  if(op[0]=='f'){ /* We got a floating op  */
    fv1 = randomFloat();
    fv2 = randomFloat();
    sprintf(buf, "%s %8.8g %8.8g\n",op,fv1,fv2);
  } else {
    iv1 = randomInt();
    iv2 = randomInt();
    sprintf(buf, "%s %d %d\n",op,iv1,iv2);
  }

    return 0;
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

  /* Do magic */
  int port=atoi(Destport);

  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;       // Allow IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;   // TCP
  hints.ai_flags = AI_PASSIVE;       // Use my IP

  int status;
  if ((status = getaddrinfo(NULL, Destport, &hints, &res)) != 0) {
      printf("getaddrinfo error:\n");
      return 1;
  }


  int serverSocket;
  int reuse = 1;


  for (p = res; p != NULL; p = p->ai_next) {
    serverSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (serverSocket < 0) {
      printf("ERROR: Socket creation failed.\n");
      continue;
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed\n");
        close(serverSocket);
        return 1;
    }

    if (bind(serverSocket, p->ai_addr, p->ai_addrlen) < 0) {
        printf("bind failed\n");
        continue;
    }
    break;
  }
  if (p == NULL) {
    return -1;
  }

  int number_of_clients_in_que = 5;
  if (listen(serverSocket, number_of_clients_in_que) == -1) {
      printf("listen\n");
      exit(EXIT_FAILURE);
  }

  struct sockaddr_storage address; 
  socklen_t addrlen = sizeof(address);
  int new_socket = accept(serverSocket, (struct sockaddr *)&address, &addrlen);
  if (new_socket < 0) {
      printf("accept failed\n");
      exit(EXIT_FAILURE);
  }
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  strcpy(buffer, "TEXT TCP 1.0\n\n");
  ssize_t bytes_sent = send(new_socket, buffer, strlen(buffer), 0);


  memset(buffer, 0, sizeof(buffer));
  recv(new_socket, &buffer, sizeof(buffer), 0);
  const char* ok_msg = "OK\n";
  printf("---------\n%s ---------\n",buffer);
  printf("---------\n%s ---------\n",ok_msg);
  if (strcmp(buffer, ok_msg) != 0) {
      printf("Client not supporting protocoll\n");
      return -1;
  }
  printf("From Client:%s",buffer);

  memset(buffer, 0, sizeof(buffer));
  get_operation(buffer);
  printf("%s\n",buffer);

#ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
#endif
  close(new_socket);
  close(serverSocket);


  return 0;
}
