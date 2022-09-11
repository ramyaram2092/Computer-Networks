#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);

 /* Use the `getaddrinfo` and `inet_ntop` functions to convert a string host and
  integer port into a string dotted ip address and port.
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid arguments - %s <host> <port>", argv[0]);
    return -1;
  }
  char* host = argv[1];
  long process = atoi(argv[2]);
  char buff[128];
  snprintf(buff,128,"%ld",process);
  char* port=buff;
 
   
  // printf("\n Host : %s",host);
  // printf("\n Port: %s",port);
  struct addrinfo hints;
  // hints=(struct addrinfo*)malloc(sizeof(struct addrinfo));
  hints.ai_flags=AI_PASSIVE;
  hints.ai_family=PF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=IPPROTO_TCP;

  struct addrinfo *response;
  response=(struct addrinfo*)malloc(sizeof(struct addrinfo));  
  
  getaddrinfo(host,port, &hints,&response);
  printf("I sucessfully contacted the server");
  return 0;
}
