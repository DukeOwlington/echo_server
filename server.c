#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define PORT 8888

int main(void) {
  struct sockaddr_in server;
  struct sockaddr_in sender;

  int st;
  unsigned int slen;
  int recv_len;
  char buf[BUFLEN];

  slen = sizeof(sender);

  /* create a UDP socket */
  if ((st = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
    return EXIT_FAILURE;
  }

  /* zero out the structure */
  memset((char *) &server, 0, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  /* bind socket to port */
  if (bind(st, (struct sockaddr*) &server, sizeof(server)) == -1) {
    perror("bind");
    return EXIT_FAILURE;
  }

  /* keep listening for data */
  while (1) {
    printf("Waiting for data...");
    fflush(stdout);

    /* try to receive some data, this is a blocking call */
    if ((recv_len = recvfrom(st, buf, BUFLEN, 0, (struct sockaddr *) &sender,
                             &slen)) == -1) {
      perror("recvfrom");
      return EXIT_FAILURE;
    }

    /* print details of the client/peer and the data received */
    printf("Received packet from %s:%d\n", inet_ntoa(sender.sin_addr),
           ntohs(sender.sin_port));
    printf("Data: %s\n", buf);

    /* now reply the client with the same data */
    if (sendto(st, buf, recv_len, 0, (struct sockaddr*) &sender, slen) == -1) {
      perror("sendto");
      return EXIT_FAILURE;
    }
  }

  close(st);
  return 0;
}
