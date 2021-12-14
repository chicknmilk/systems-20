#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {

  mkfifo(WKP, 0644);

  int from_client = open(WKP, O_RDONLY);

  char * buffer = calloc(BUFFER_SIZE, sizeof(char));
  read(from_client, buffer, BUFFER_SIZE);
  printf("Secret: %s\n", buffer);
  remove(WKP);

  *to_client = open(buffer, O_WRONLY);
  write(*to_client, ACK, strlen(ACK));

  char * res = calloc(BUFFER_SIZE, sizeof(char));
  read(from_client, res, BUFFER_SIZE);
  printf("Recieved response: %s\n", res);

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  
  int from_server = 0;

  char buffer[BUFFER_SIZE];
  int pid = getpid();
  sscanf(buffer, "%d", &pid);
  mkfifo(buffer, 0644);

  *to_server = open(WKP, O_WRONLY);
  write(*to_server, buffer, strlen(buffer));

  from_server = open(buffer, O_RDONLY);
  char * res = calloc(BUFFER_SIZE, sizeof(char));
  read(from_server, res, BUFFER_SIZE);

  printf("Recieved response: %s\n", res);
  remove(buffer);

  if (!strcmp(res, ACK)) {
    write(*to_server, "client msg", strlen("client msg"));
  }

  return from_server;
}
