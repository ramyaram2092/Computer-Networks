#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>

const char *inet_ntop(int af, const void *restrict src,
                      char *restrict dst, socklen_t size);
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
  char* port = argv[2]; // atoi=> char* to integer
  // char buff[128];
  // snprintf(buff,128,"%ld",process);
  // char* port=buff;
 
   
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
  // printf("I sucessfully contacted the server");
  //-> indirection
  struct addrinfo *iterator=response;
  
  while(iterator!=NULL)
  {
    void * raw_addr;
    char buffer[4096];

    if(iterator->ai_family==AF_INET)// Address is IPv4
    {
      struct sockaddr_in* tmp=(struct sockaddr_in*)iterator->ai_addr;
      raw_addr= &(tmp->sin_addr);
      inet_ntop(iterator->ai_family,raw_addr,buffer,4096);
      printf("IPv4 %s\n",buffer);


    }
    else // Address is IPv6
    {
      struct sockaddr_in6* tmp=(struct sockaddr_in6*)iterator->ai_addr;
      raw_addr=&(tmp->sin6_addr);
      inet_ntop(iterator->ai_family,raw_addr,buffer,4096);
      printf("IPv6 %s\n",buffer);
    }
    iterator=iterator->ai_next;
    

  }
  


  return 0;
}
