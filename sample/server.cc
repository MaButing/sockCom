// Server:
// Compile:  g++ server.cpp -o server
// Usage:    ./server &

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>

/*
* Note: Could use struct addrinfo to help choose our bind parameters.
*       Chose not to for simplicity, but addrinfo recommended for IPv6.
*/

/*
* This program creates a socket and waits for clients to connect. It displays
* there messages and repeats after the connection is closed.
*/
int main()
{
  int sock;
  socklen_t length;
  struct sockaddr_in server;
  int msgsock;
  char buf[1024];
  int rval;

  // Create socket.
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    perror("opening stream socket");
    exit(1);
  }

  // Use wildcards to set up for bind.
  server.sin_family = AF_INET;          // Internet IP Protocol (v4)
  server.sin_addr.s_addr = INADDR_ANY;  // Bind to all local interfaces
  server.sin_port = htons(6666);                  // Will be set by bind

  // Do bind. This assigns the socket a "name."
  if (bind(sock, (struct sockaddr *) &server, sizeof server) == -1)
  {
    perror("binding stream socket");
    exit(1);
  }

  // Find out assigned port number and print it out.
  length = sizeof server;
  if (getsockname(sock, (struct sockaddr *) &server, &length) == -1)
  {
    perror("getting socket name");
    exit(1);
  }

  printf("Socket port #%d\n", ntohs(server.sin_port));

  // Mark socket as a server/listener.
  listen(sock, 5);

  // Start accepting connections.
  while (1) {
    // Grab the next connection.
    msgsock = accept(sock,(struct sockaddr *) 0,(socklen_t *) 0);
    if (msgsock == -1)
    {
      perror("accept");
      exit(1);
    }
    printf("New connection\n");

    // Keep receiving 1024 byte chunks until we've read everything.
    do {
      memset(buf, 0, sizeof buf); // just wipes the buffer
      if ((rval = recv(msgsock, buf, 1024, 0)) == -1)
        perror("reading stream message");
      if (rval == 0)
        printf("Ending connection\n");
      else
        printf("-->%d,%s\n", rval, buf);
    } while (rval != 0);

    close(msgsock);
  }

  // Code never gets here.
  // But this is how we would close a socket.
  close(sock);
  return 0;
}
