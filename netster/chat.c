#include<sys/socket.h>
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>



void  server_udp(char* iface, long port);
void  server_tcp(char* iface, long port);
void  client_tcp(char* host, long port);
char *inet_ntoa(struct in_addr in);

int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);
int serverchatHandler(int socketFileDescriptor);
void clientchatHandler(int socketFileDescriptor);


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
    printf("\n");
    

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
  if(serverSocket<0)
  {
    printf("\n server socket creation failed \n");
  }



  // assign ip and port

  server.sin_family=AF_INET; // address family IPV4 or 6
  server.sin_addr.s_addr= INADDR_ANY ;// takes default ip-> local ip
  server.sin_port=port;


  //bind the socket with the server ip and port 
  if ((bind(serverSocket,(struct sockaddr*) & server, sizeof(server)))!=0)
  {
    printf("\n Error in socket binding \n ");
    exit(0);
  }

  
  for (;;)
  {

  // listen to the socket connection 
  if(listen(serverSocket,3)!=0) // the no 5 is subjected to change. no of requests that can be queued
  {
    printf("\n Listening failed\n ");
    exit(0);

  }

    printf("\n Server is listening to socket\n ");
  

  socklen_t clientLen= sizeof(client);

  // accept the incoming packet from client 
  
   newSocket=accept(serverSocket, (struct sockaddr*) &client, &clientLen );
   if(newSocket<0)
   {
      printf("\n Unable to accept the client packet\n ");
      exit(0);

   }
    // handle the chat with client
    if(serverchatHandler(newSocket)==-1)
    {
      break;
    }

  }

    //close the socket 
    close (serverSocket);
    
}



/**
 * @brief server's chat handler function
 * 
 * @param socketFileDescriptor 
 */

int serverchatHandler(int socketFileDescriptor)
{
   char message[100];

   for(;;)
   { 
      bzero(message, sizeof(message));

      // recieve message if any 
      if((recv(socketFileDescriptor,message,sizeof(message),0)<0))
      {
        printf("coundnt recieve message from client \n");
        exit(0);
      }

        // display the recieved message 
      printf("Message recieved from client :  %s \n ", message);
      
      int len= (int)strlen(message)-1;
      
      // case 1: if the client sends "exit", send ok and  server should exit 
      if((strncmp(message,"exit",len))==0)
      {
        if((send(socketFileDescriptor,"ok",strlen("ok"),0))<0)
        {
          printf("Sending message from server failed\n");
          exit(0);
        }
        printf("Server exits...\n");
        return -1;
      }

      // case 2: if client sends "goodbye"  send farewell and disconnect from the client 
      else if((strncmp(message,"goodbye",len))==0)
      {
        if((send(socketFileDescriptor,"farewell",strlen("farewell"),0))<0)
        {
          printf("Sending message from server failed\n");
          exit(0);
        }
        printf("Server Exiting the current client ...\n");
        return 0;
      }

      else
      {
        // case 1: if the client sends "hello" respond with world 
        if(strncmp(message,"hello",len)==0)
        {
          if(send(socketFileDescriptor,"world",strlen("world"),0)<0)
          {
            printf("Sending message from server failed\n");
            exit(0);
          }
        }
        else
        {
          bzero(message, sizeof(message));
          // read the input 
          int i=0;
          while ((message[i++] = getchar()) != '\n')
          ;
          message[i]='\0';
          //send  the server's response to client 
          if((send(socketFileDescriptor,message,strlen(message),0))<0)
          {
            printf("Sending message from server failed\n");
            exit(0);

          }
        }
      }
   }
  return 0;      
   
}

/**
 * @brief tcp client
 * 
 * @param host 
 * @param port 
 */
void client_tcp(char* host, long port)
{
   int clientSocket;
   struct sockaddr_in serveraddr;
  //  printf("\n Port : %ld",port);

  //create socket
  clientSocket=socket(AF_INET,SOCK_STREAM,0);
  if(clientSocket<0)
  {
    printf("\n Client socket creation failed \n");
    exit(0);
  }
   
  // assign ip and port
  serveraddr.sin_family=AF_INET; // address family IPV4 or 6
  serveraddr.sin_addr.s_addr= inet_addr(host);
  serveraddr.sin_port=port;

      

  //connect client socket with  server socket 
  if(connect(clientSocket,(struct sockaddr*)&serveraddr, sizeof(serveraddr) )!=0)
  {
      printf("\n Connection with server failed \n ");
      exit(0);

  }

  // Try to send a message to server
  clientchatHandler(clientSocket);  
  close (clientSocket);

}

/**
 * @brief client's chat handler
 * 
 * @param socketFileDescriptor 
 */

void clientchatHandler(int socketFileDescriptor)
{
  char message[100];// The
  for(;;)
  {

        bzero(message, sizeof(message));
        // read input from user 
        int i=0;
        while ((message[i++] = getchar()) != '\n')
        ;
        message[i]='\0';
        if(send(socketFileDescriptor,message,strlen(message),0)<0)
        {
            printf("Sending message from client failed\n ");
            exit(0);
        }
        bzero(message, sizeof(message));  
        // recieve message if any 
        if((recv(socketFileDescriptor,message,sizeof(message),0)<0))
        {
            printf("Recieving message from Server failed \n");
            exit(0);
        }
        printf("Message recived from server : %s\n ", message);
        int len= (int)strlen(message)-1;


        if((strncmp(message,"farewell",len)==0)||(strncmp(message,"ok",len)==0))
        {
            printf("Client Exit...\n");
            break;
        }
    }

          
      
  }


























