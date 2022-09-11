#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>

/*
  Use the `getaddrinfo` and `inet_ntop` functions to convert a string host and
  integer port into a string dotted ip address and port.
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid arguments - %s <host> <port>", argv[0]);
    return -1;
  }
  char* host = argv[1];
  long process = atoi(argv[2]);
  // char buff[128];
  // int n=snprintf(buff,128,"%ld",process);
  // char* port=buff;
  char buff [128];
_itoa_s(process,buff,sizeof(buff),10);

  printf("\n Host : %s",host);
  printf("\n Port: %s",buff);
  

  return 0;
}
