# include<sys/socket.h>

#define BACKLOG 10
void  server_udp(char* iface, long port);
void  server_tcp(char* iface, long port);
void client_tcp(char* host, long port);


/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void chat_server(char* iface, long port, int use_udp) {
    if(use_udp==0)
    {
        server_tcp(iface,port);
    }
    else
    {
        server_udp(iface,port);
    }

}

void chat_client(char* host, long port, int use_udp) {
  if(use_udp==0)
    {
        client_tcp(host,port);
    }
    else
    {
        client_udp(host,port);
    }
  
}

// for udp server 
/**
 1. Create a UDP socket.
2. Bind the socket to the server address.
3. Wait until the datagram packet arrives from the client.
4. Process the datagram packet and send a reply to the client.
5. Go back to Step 3.
 */
void  server_udp(char* iface, long port)
{
    // int socket_fd, new_fd; // listen on sockfd, new connection on new_fd
    // struct sockaddr_in server_addr; // my address information
    // struct sockaddr_in client_addr; // client's address information

    // if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    // {
    //     perror("socket");
    //     exit(1);
    // }
    // server_addr.sin_family = AF_INET; // host byte order
    // server_addr.sin_port = htons(port); // short, network byte order
    // server_addr.sin_addr.s_addr = INADDR_ANY; // auto. filled with local IP
    // memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct

    // if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) 
    // {
    //     perror("bind");
    //     exit(1);
    // }
    // printf("\n Hello, I am a server");

}

// for tcp server 
/**
1. using create(), Create TCP socket.
2. using bind(), Bind the socket to server address.
3. using listen(), put the server socket in a passive mode, where it waits for the client to approach the server to make a connection
4. using accept(), At this point, connection is established between client and server, and they are ready to transfer data.
5. Go back to Step 3. 
 */
void  server_tcp(char* iface, long port)
{
  int serverSocket, clientSocket;
  struct sockaddr_in server,client;

  // create socket 
  serverSocket=socket(AF_INET,SOCK_STREAM,0);
  if(socket==-1)
  {
    printf("\n server socket creation failed");
  }
  else
  {
    printf("\n Server Socket created Successfully");
  }

   memset(&server,0,sizeof(server)); // appending zero ?  Read about it
  // assign ip and port

  server.sin_family=AF_INET; // address family IPV4 or 6
  server.sin_addr.s_addr= INADDR_ANY  // takes default ip-> local ip
  server.sin_port=(int)port;


  //bind the socket with the server ip and port 
  if ((bind(serverSocket,(struct sockaddr*) & server, sizeOf(server)))<0)
  {
    printf("Error in socket binding");
  }
  else
  {
    printf("Socket successfully binded to server ");
  }


  // listen to the socket connection 
  if(listen(serverSocket,5)<0) // the no 5 is subjected to change. no of requests that can be queued
  {
    printf("\n Listening failed");
  }
  else
  {
    printf("\n Server is listening to socket");
  }

  int clientLen= sizeOf(client);
  // accept the incoming packet from client 
   clientSocket=accept(serverSocket, (struct sockaddr*) &client, & clientLen );
   if(clientPacket<0)
   {
      printf("\n Unable to accept the client packet");
   }
   else{
    printf("\n Suceessfully accepted the client packet from %n", inet_ntoa(client.sin_addr));
   }

   if((send(clientSocket,"Hi this is server\n",14,0))<0)
   {
    printf("Sending message from server failed");
   }
   else
   {
    printf("message sent successfully");
   }
   close(clientSocket);

   close (serverSocket);
    
}


void client_tcp(char* host, long port)
{
   int serverSocket, clientSocket;
  //  struct sockaddr_in server,client;
   char buffer[128];
   struct addrinfo server;
   struct addrinfor* response;
   
   
   server.ai_family=AF_INET;
   server.ai_socktype=SOCK_STREAM;
   server.ai_protocol=IPPROTO_TCP;

   getaddrinfo(host,port,&server,&response);
   printf("\n This is client program");


}


















// socket(AF_INET, SOCK_STREAM, 0)) creates a socket connection  in the port we are listening 
// MYPORT is the port value that we are recieving in the function call


// #define MYPORT 3490 // the port users will be connecting to
// #define BACKLOG 10 // how many pending connections queue will hold
// int main(void) {
// int sockfd, new_fd; // listen on sockfd, new connection on new_fd
// struct sockaddr_in server_addr; // my address information
// struct sockaddr_in their_addr; // connector's address information
// int sin_size;
// if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
// perror("socket");
// exit(1);
// }
// server_addr.sin_family = AF_INET; // host byte order
// server_addr.sin_port = htons(MYPORT); // short, network byte order
// server_addr.sin_addr.s_addr = INADDR_ANY; // auto. filled with local IP
// memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct


// if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
// perror("bind");
// exit(1);
// }
// if (listen(sockfd, BACKLOG) == -1) {
// perror("listen");
// exit(1);
// }
// while(1) { // main accept() loop
// sin_size = sizeof(struct sockaddr_in);
// if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
// perror("accept");
// continue;
// }
// printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
// if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
// perror("send");
// close(new_fd);
// }
// return





/******************************************************
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
// int main(int argc, char* argv[]) {
//   if (argc != 3) {
//     printf("Invalid arguments - %s <host> <port>", argv[0]);
//     return -1;
//   }
//   char* host = argv[1];
//   char* port = argv[2]; // atoi=> char* to integer
//   // char buff[128];
//   // snprintf(buff,128,"%ld",process);
//   // char* port=buff;
 
   
//   // printf("\n Host : %s",host);
//   // printf("\n Port: %s",port);
//   struct addrinfo hints;
//   // hints=(struct addrinfo*)malloc(sizeof(struct addrinfo));
//   hints.ai_flags=AI_PASSIVE;
//   hints.ai_family=PF_UNSPEC;
//   hints.ai_socktype=SOCK_STREAM;
//   hints.ai_protocol=IPPROTO_TCP;

//   struct addrinfo *response;
//   response=(struct addrinfo*)malloc(sizeof(struct addrinfo));  
  
//   getaddrinfo(host,port, &hints,&response);
//   // printf("I sucessfully contacted the server");
//   //-> indirection
//   struct addrinfo *iterator=response;
  
//   while(iterator!=NULL)
//   {
//     void * raw_addr;
//     char buffer[4096];

//     if(iterator->ai_family==AF_INET)// Address is IPv4
//     {
//       struct sockaddr_in* tmp=(struct sockaddr_in*)iterator->ai_addr;
//       raw_addr= &(tmp->sin_addr);
//       inet_ntop(iterator->ai_family,raw_addr,buffer,4096);
//       printf("IPv4 %s\n",buffer);


//     }
//     else // Address is IPv6
//     {
//       struct sockaddr_in6* tmp=(struct sockaddr_in6*)iterator->ai_addr;
//       raw_addr=&(tmp->sin6_addr);
//       inet_ntop(iterator->ai_family,raw_addr,buffer,4096);
//       printf("IPv6 %s\n",buffer);
//     }
//     iterator=iterator->ai_next;
    

//   }
  


//   return 0;
// }
