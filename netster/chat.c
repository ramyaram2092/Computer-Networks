#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>

struct clientDetails
{
  int socketfileDesctiptor;
  char *host;
  long port;
  int serverSocket;
} cd;

void server_udp(char *iface, long port);
void server_tcp(char *iface, long port);
void client_tcp(char *host, long port);
void client_udp(char *host, long port);

char *inet_ntoa(struct in_addr in);

int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);
void *serverchatHandler(void *);
void clientchatHandler(int socketFileDescriptor);

void *printServer(void *);

/*
 *  Here is the starting point for your netster part.1 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void chat_server(char *iface, long port, int use_udp)
{
  if (use_udp == 0)
  {

    server_tcp(iface, port);
  }
  else
  {
    // printf("yooo");
    server_udp(iface, port);
  }
}

void chat_client(char *host, long port, int use_udp)
{
  if (use_udp == 0)
  {
    client_tcp(host, port);
  }
  else
  {
    client_udp(host, port);
  }
}

void server_udp(char *iface, long port)
{
  int serverSocket;
  struct sockaddr_in server, client;

  // create socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    printf("Server socket creation failed \n");
  }


  // assign ip and port

  server.sin_family = AF_INET;         // address family IPV4 or 6
  server.sin_addr.s_addr = INADDR_ANY; // takes default ip-> local ip
  server.sin_port = htons(port);

  // bind the socket with the server ip and port
  if ((bind(serverSocket, (struct sockaddr *)&server, sizeof(server))) < 0)
  {
    printf("\n Error in socket binding \n ");
    exit(0);
  }

  // sample code
  socklen_t len;
  char buffer[256];
  char *hello = "Hello from server";
  len = sizeof(client);
  printf("\n UDP Port:%ld",port);

  int recv_msg_client = recvfrom(serverSocket, buffer, 256, MSG_WAITALL, (struct sockaddr *)&client, &len);

  if (recv_msg_client < 0) {
    printf("\n SOmething went wrong while recv  the msg from  client");
    return;
  }
  // printf("n: %d", (int)n);
  // buffer[(int)n] = '\0';
  printf("Client : %s\n", buffer);
  sendto(serverSocket, hello, strlen(hello), 0, (struct sockaddr *)&client, len);
  printf("%s message sent.\n", hello);
}

void client_udp(char *host, long port)
{
  int clientSocket;
  struct sockaddr_in serveraddr;

  // create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0)
  {
    printf("Client socket creation failed \n");
    exit(0);
  }


  // assign ip and port
  serveraddr.sin_family = AF_INET; // address family IPV4 or 6
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serveraddr.sin_port = htons(port);

  // Try to send a message to server

  int n;
  char buffer[256];
  socklen_t len;
  char hello[256]="Hey this is client";
  printf("\n Port:%ld",port);
  int sent_msg_flag = sendto(clientSocket, hello, 256, 0, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
  printf("%s message sent.\n", hello);
  if (sent_msg_flag < 0) {
    printf("\n SOmething went wrong while send the msg");
    return;
  }
  // printf("%s message sent.\n", hello);

  n = recvfrom(clientSocket, buffer, 256, MSG_WAITALL, (struct sockaddr *)&serveraddr,
               &len);
  buffer[n] = '\0';
  printf("Server : %s\n", buffer);

  close(clientSocket);
  
}




void server_tcp(char *iface, long port)
{

  int serverSocket, newSocket;
  struct sockaddr_in server, client;

  // create socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    printf("\n server socket creation failed \n");
  }

  // assign ip and port

  server.sin_family = AF_INET;         // address family IPV4 or 6
  server.sin_addr.s_addr = INADDR_ANY; // takes default ip-> local ip
  server.sin_port = port;

  // bind the socket with the server ip and port
  if ((bind(serverSocket, (struct sockaddr *)&server, sizeof(server))) != 0)
  {
    printf("\n Error in socket binding \n ");
    exit(0);
  }

  int i = 0;
  for (;;)
  {
    // printf("Listening for next conncection \n");

    // listen to the socket connection
    if (listen(serverSocket, 3) != 0) // the no 5 is subjected to change. no of requests that can be queued
    {
      printf("\n Listening failed\n ");
      exit(0);
    }

    socklen_t clientLen = sizeof(client);

    // accept the incoming packet from client

    newSocket = accept(serverSocket, (struct sockaddr *)&client, &clientLen);

    if (newSocket < 0)
    {
      printf("\n Unable to accept the client packet\n ");
      exit(0);
    }
    char buffer[200];
    inet_ntop(AF_INET, &client.sin_addr.s_addr, buffer, 200);
    printf("Connection %d from (%s,%ld) \n ", i, buffer, port);
    pthread_t id;

    struct clientDetails cd;
    cd.host = buffer;
    cd.port = port;
    cd.socketfileDesctiptor = newSocket;
    cd.serverSocket = serverSocket;

    // handle the chat with client
    pthread_create(&id, NULL, serverchatHandler, &cd);

    i++;
    // pthread_kill(id,0);
  }

  // close the socket
  close(serverSocket);
}

/**
 * @brief server's chat handler function
 *
 * @param socketFileDescriptor
 */

void *serverchatHandler(void *argp)
{
  struct clientDetails *c = (struct clientDetails *)argp;
  char *host = c->host;
  long port = c->port;
  int socketFileDescriptor = c->socketfileDesctiptor;
  // pthread_t id=c->id;
  char message[200];

  for (;;)
  {
    bzero(message, sizeof(message));

    // recieve message if any
    if ((recv(socketFileDescriptor, message, sizeof(message), 0) < 0))
    {
      printf("coundnt recieve message from client \n");
      // exit(0);
    }

    // display the recieved message
    printf("got message from (%s,%ld)\n", host, port);

    int len = (int)strlen(message) - 1;

    char client_msg[200];
    int j = 0;
    while (message[j])
    {
      char ch = toupper(message[j]);
      client_msg[j++] = ch;
    }
    client_msg[j] = '\0';
    // printf("%s",client_msg);

    // case 1: if the client sends "exit", send ok and  server should exit
    if ((strncmp(client_msg, "EXIT", len)) == 0)
    {
      if ((send(socketFileDescriptor, "ok", strlen("ok"), 0)) < 0)
      {
        printf("Sending message from server failed\n");
        exit(0);
      }
      // c->status=-1;
      // return (void*)-1;
      // pthread_exit(NULL);
      exit(0);
    }

    // case 2: if client sends "goodbye"  send farewell and disconnect from the client
    else if ((strncmp(client_msg, "GOODBYE", len)) == 0)
    {
      if ((send(socketFileDescriptor, "farewell", strlen("farewell"), 0)) < 0)
      {
        printf("Sending message from server failed\n");
        exit(0);
      }
      break;
    }

    else
    {
      // case 1: if the client sends "hello" respond with world
      if (strncmp(client_msg, "HELLO", len) == 0)
      {
        if (send(socketFileDescriptor, "world", strlen("world"), 0) < 0)
        {
          printf("Sending message from server failed\n");
          exit(0);
        }
      }
      else
      {
        bzero(message, sizeof(message));
        // read the input
        int i = 0;
        while ((message[i++] = getchar()) != '\n')
          ;
        message[i] = '\0';
        // send  the server's response to client
        if ((send(socketFileDescriptor, message, strlen(message), 0)) < 0)
        {
          printf("Sending message from server failed\n");
          exit(0);
        }
      }
    }
  }
  fflush(stdout);

  return NULL;
}

/**
 * @brief tcp client
 *
 * @param host
 * @param port
 */
void client_tcp(char *host, long port)
{
  int clientSocket;
  struct sockaddr_in serveraddr;
  //  printf("\n Port : %ld",port);

  // create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0)
  {
    printf("Client socket creation failed \n");
    exit(0);
  }

  // assign ip and port
  serveraddr.sin_family = AF_INET; // address family IPV4 or 6
  serveraddr.sin_addr.s_addr = inet_addr(host);
  serveraddr.sin_port = port;

  // connect client socket with  server socket
  if (connect(clientSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
  {
    printf("Connection with server failed \n ");
    exit(0);
  }

  // Try to send a message to server
  clientchatHandler(clientSocket);
  close(clientSocket);
}

/**
 * @brief client's chat handler
 *
 * @param socketFileDescriptor
 */

void clientchatHandler(int socketFileDescriptor)
{
  char message[100]; // The
  for (;;)
  {

    bzero(message, sizeof(message));
    // read input from user
    int i = 0;
    while ((message[i++] = getchar()) != '\n')
      ;
    message[i] = '\0';
    if (send(socketFileDescriptor, message, strlen(message), 0) < 0)
    {
      printf("Sending message from client failed\n ");
      exit(0);
    }
    bzero(message, sizeof(message));
    // recieve message if any
    if ((recv(socketFileDescriptor, message, sizeof(message), 0) < 0))
    {
      printf("Recieving message from Server failed \n");
      exit(0);
    }
    printf("%s\n", message);
    int len = (int)strlen(message) - 1;

    if ((strncmp(message, "farewell", len) == 0) || (strncmp(message, "ok", len) == 0))
    {
      printf("Client Exit...\n");
      break;
    }
  }
}
