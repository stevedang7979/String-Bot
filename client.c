#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 8192 /* Max text line length */

int open_clientfd(char* netid, int port, char *hostname) {
  // The client's socket file descriptor.
  int clientfd;
  // The hostent struct is used to get the IP address of the server using DNS.
  struct hostent *hp;
  // serveraddr is used to record the server information (IP address and port number).
  struct sockaddr_in serveraddr;
  printf("Echo Client is creating a socket.\n");
  // First, we create the socket file descriptor with the given
  // protocol and protocol family.
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
  // Query DNS for the host (server) information.
  if ((hp = gethostbyname(hostname)) == NULL) return -2;
  // The socket API requires that you zero out the bytes!
  bzero((char *)&serveraddr, sizeof(serveraddr));
  // Record the protocol family we are using to connect.
  serveraddr.sin_family = AF_INET;
  // Copy the IP address provided by DNS to our server address structure.
  bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr,hp->h_length);
  // Convert the port from host byte order to network byte order and
  // store this in the server address structure.
  serveraddr.sin_port = htons(port);
  printf("Echo Client is trying to connect to %s (%s:%d).\n", hostname, inet_ntoa(serveraddr.sin_addr), port);
  // Establish a connection with the server.
  if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    return -1;
  printf("Echo Client connected.\n");
  // Return the connected file descriptor.
  return clientfd;
}

int main(int argc, char **argv) {
  // The client socket file descriptor.
  int clientfd;

  // The port number.
  int port;

  //netid@umass.edu
  char *netid;

  // Variable to store the host/server domain name.
  char *host;

  // A buffer to receive data from the server.
  char buf[MAXLINE];

  // First, we check the program arguments:
  if (argc != 4) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }

  // First argument is netid, second is port, third is host
  netid = argv[1];
  port = atoi(argv[2]);
  host = argv[3];
  
  // Open the client socket file descriptor given the netid, port, and host:
  clientfd = open_clientfd(netid, port, host);

  // Print "type: " and fflush to stdout:
  printf("please type: ");
  fflush(stdout);
  //
    //fgets(buf, MAXLINE, stdin);
    strncpy(buf, "cs230 HELLO netid@umass.edu\n", MAXLINE);
    ssize_t ns = send(clientfd, buf, strlen(buf), 0);
    ssize_t nr = recv(clientfd, buf, MAXLINE, 0);
    // Display:
    printf("echo (%ld bytes): ", nr);
    fputs(buf, stdout);
    fflush(stdout);
    //
    char cs230[6];
    char status[6];
    int shift,byteRead;
    char msg[50];
    sscanf(buf, "%s %s %d %s", cs230, status, &shift, msg);
    char head[57]= {'c','s','2','3','0',' '};
    for(int i=0; i < strlen(msg); i++){
      head[6+i] = 65 + (( (int)(msg[i]) + shift -65) % 26);
    }
    head[56]='\n';



    //iteration here:
    while(nr != 0){

    ns = send(clientfd, head, strlen(head), 0);
    nr = recv(clientfd, buf, MAXLINE, 0);
    // Display:
    printf("echo (%ld bytes): ", nr);
    fputs(buf, stdout);
    //
    sscanf(buf, "%s %s %d %s", cs230, status, &shift, msg);
    for(int j=0; j < strlen(msg); j++){
      head[6+j] = 65 + (( (int)(msg[j]) + shift -65) % 26);
    }
}

  // Close the file descriptor:
  close(clientfd);
  exit(0);
}
