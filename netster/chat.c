#include<sys/socket.h>
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>



#define BACKLOG 10
void  server_udp(char* iface, long port);
void  server_tcp(char* iface, long port);
void  client_tcp(char* host, long port);
char *inet_ntoa(struct in_addr in);

int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);

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
        // client_udp(host,port);
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
  int serverSocket, newSocket;
  struct sockaddr_in server,client;

  // create socket 
  serverSocket=socket(AF_INET,SOCK_STREAM,0);
  if(serverSocket==-1)
  {
    printf("\n server socket creation failed \n");
  }
  else
  {
    printf("\n Server Socket created Successfully\n ");
  }

   memset(&server,0,sizeof(server)); // appending zero ?  Read about it

  // assign ip and port

  server.sin_family=AF_INET; // address family IPV4 or 6
  server.sin_addr.s_addr= INADDR_ANY ;// takes default ip-> local ip
  server.sin_port=(int)port;


  //bind the socket with the server ip and port 
  if ((bind(serverSocket,(struct sockaddr*) & server, sizeof(server)))<0)
  {
    printf("\n Error in socket binding \n ");
    exit(0);
  }
  else
  {
    printf("\n Socket successfully binded to server\n ");
  }


  // listen to the socket connection 
  if(listen(serverSocket,3)<0) // the no 5 is subjected to change. no of requests that can be queued
  {
    printf("\n Listening failed\n ");
  }
  else
  {
    printf("\n Server is listening to socket\n ");
  }

  socklen_t clientLen= sizeof(client);

  // accept the incoming packet from client 
   newSocket=accept(serverSocket, (struct sockaddr*) &client, &clientLen );
   if(newSocket<0)
   {
      printf("\n Unable to accept the client packet\n ");
   }
   else{
    printf("\n Suceessfully accepted the client packet from %s", inet_ntoa(client.sin_addr));
   }

   if((send(newSocket,"Hi this is server\n",14,0))<0)
   {
    printf("\n Sending message from server failed\n ");
   }
   else
   {
    printf("\n message sent successfully\n ");
   }
   close(newSocket);

   close (serverSocket);
    
}


void client_tcp(char* host, long port)
{
   int clientSocket;
   struct sockaddr_in server;

  //create socket
  clientSocket=socket(AF_INET,SOCK_STREAM,0);
  if(clientSocket<0)
  {
    printf("\n Client socket creation failed \n");
    exit(0);
  }
  else
  {
    printf("\n Client Socket created Successfully\n ");
  }
   
  

  // assign ip and port
  

  server.sin_family=AF_INET; // address family IPV4 or 6
  server.sin_addr.s_addr= inet_addr(host);
  server.sin_port=(int)port;

  memset(&server,0,sizeof(server)); // appending zero ?  Read about it

  //connect client socket with  server socket 
   while (1)
  {
    if((connect(clientSocket,(struct sockaddr*)&server, sizeof(server) ))<0)
    {
      printf("\n Connection with server failed \n ");
    }
    else
    {
      printf("\n Finally connected successfully to server \n ");
      printf("\n I coem here \n  ");

    }
    printf("\n I am coming here though? ");


    // Try to send a message to server
  
    if((send(clientSocket,"Hi this is Client. Nice to meet you \n",100,0))<0)
    {
      printf("\n Sending message from client failed\n ");
    }
    else
    {
      printf("\n message sent from client  successfully\n ");
    }
  }

   printf("\n I am coming here ");


  // close (clientSocket);



}



















