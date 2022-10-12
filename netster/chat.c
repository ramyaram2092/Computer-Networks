#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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

  // Create a socket
  int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (serverSocket < 0)
  {
    printf("Problem creating socket - UDP");
    return;
  }
  struct sockaddr_in server;

  // Set the server information (Internet family, port, and ipaddress)
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  /*binds server information (Internet family, port, and ipaddress)
   to server socket*/
  int bind_successful_flag = bind(serverSocket,
                                  (struct sockaddr *)&server,
                                  sizeof(server));
  if (bind_successful_flag < 0)
  {
    printf("Socket binding failed - UDP");
    return;
  }
  struct sockaddr_in client_info;
  socklen_t client_info_length = sizeof(client_info);
  char clientmsg[256];
  char servermsg[256];
  char client_description_two[256];
  int close_client_connection_flag = 0; // flag for managing new connections
  while (1)
  {
    // receive client message
    int received_message_flag = recvfrom(serverSocket, clientmsg, 256,
                                         MSG_WAITALL,
                                         (struct sockaddr *)&client_info,
                                         &client_info_length);
    if (received_message_flag < 0)
    {
      printf("Error occured while receiving the message - UDP");
      return;
    }
    snprintf(client_description_two,
             sizeof(client_description_two),
             "%s%s%s%s%d%s", "got message from ",
             "('", inet_ntoa(client_info.sin_addr), "', ",
             ntohs(client_info.sin_port), ")");
    printf("%s\n", client_description_two);

    // logic for managing custom user messages
    if (strcmp(clientmsg, "hello\n") == 0)
    {
      strcpy(servermsg, "world\n");
    }
    else if (strcmp(clientmsg, "goodbye\n") == 0)
    {
      strcpy(servermsg, "farewell\n");
    }
    else if (strcmp(clientmsg, "exit\n") == 0)
    {
      strcpy(servermsg, "ok\n");
      close_client_connection_flag = 1;
    }
    else
    {
      strcpy(servermsg, clientmsg);
    }
    // send message to client
    int send_message_flag = sendto(serverSocket, servermsg, 256, 0,
                                   (const struct sockaddr *)&client_info,
                                   sizeof(client_info));
    if (send_message_flag < 0)
    {
      printf("Error occured while sending the message - UDP");
      return;
    }
    /*logic for new client connection, after previous client
        connection has been closed*/
    if (close_client_connection_flag == 1)
    {
      break;
    }

    // reset clientmsg & servermsg arrays
    memset(clientmsg, '\0', sizeof(clientmsg));
    memset(servermsg, '\0', sizeof(servermsg));
  }
  close(serverSocket);
}

void client_udp(char *host, long port)
{
  int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (clientSocket < 0)
  {
    printf("Problem creating socket - UDP");
    return;
  }
  struct sockaddr_in server;
  socklen_t serverSize = sizeof(server);
  /*Set the server information (Internet family, port, and ipaddress)
  - same as server*/
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(host);
  // printf("%u", server.sin_addr.s_addr);

  char clientMessage[256];
  char serverMessage[256];
  while (1)
  {
    // get client input
    char *result;
    result = fgets(clientMessage, 256, stdin);

    if (result == NULL)
    {
      printf("Something went wrong exiting...\n");
      return;
    }
    // /strlen(clientmsg)
    int send_message_flag = sendto(clientSocket, clientMessage, 256, 0, (const struct sockaddr *)&server, sizeof(server));
    if (send_message_flag < 0)
    {
      printf("Unable to send message to the server - UDP");
      return;
    }
    // Reveive message from the server  sizeof(servermsg),
    int servermsg_flag = recvfrom(clientSocket, serverMessage, 256, MSG_WAITALL, (struct sockaddr *)&server, &serverSize);
    if (servermsg_flag < 0)
    {
      printf("Problem in receiving server message - UDP");
      return;
    }

    // print server message
    printf("%s", serverMessage);
    if (strcmp(serverMessage, "farewell\n") == 0 ||
        (strcmp(clientMessage, "exit\n") == 0 && strcmp(serverMessage, "ok\n") == 0))
    {
      break;
    }

    // reset clientmsg & servermsg arrays
    memset(clientMessage, '\0', sizeof(clientMessage));
    memset(serverMessage, '\0', sizeof(serverMessage));
  }
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
    printf("server socket creation failed \n");
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
