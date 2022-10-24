#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

void tcp_server_ft(char *iface, long port, FILE *fp);
void tcp_client_ft(char *host, long port, FILE *fp);
void udp_server_ft(char *iface, long port, FILE *fp);
void udp_client_ft(char *host, long port, FILE *fp);

const char *inet_ntop(int af, const void *restrict src,
                      char *restrict dst, socklen_t size);

void file_server(char *iface, long port, int use_udp, FILE *fp)
{
    if (use_udp == 0)
    {
        tcp_server_ft(iface, port, fp);
    }
    else
    {
        udp_server_ft(iface, port, fp);
    }
}

void file_client(char *host, long port, int use_udp, FILE *fp)
{
    if (use_udp == 0)
    {
        tcp_client_ft(host, port, fp);
    }
    else
    {
        udp_client_ft(host, port, fp);
    }
}
/**
 * @brief TCP server
 *
 * @param iface
 * @param port
 * @param fp
 */
void tcp_server_ft(char *iface, long port, FILE *fp)
{
    int serverSocket, newSocket;
    struct sockaddr_in server, client;

    // create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        printf("TCP: Server socket creation failed \n");
    }

    // resolve address into IP

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;

    struct addrinfo *response;
    response = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    char str[256];
    sprintf(str, "%ld", port);
    getaddrinfo(iface, str, &hints, &response);
    struct addrinfo *iterator = response;
    char buffer[4096];
    void *raw_addr;

    struct sockaddr_in *tmp = (struct sockaddr_in *)iterator->ai_addr;
    raw_addr = &(tmp->sin_addr);
    inet_ntop(AF_INET, raw_addr, buffer, 4096);

    // assign ip and port

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(buffer);
    server.sin_port = htons(port);

    // bind the socket with the server ip and port
    if ((bind(serverSocket, (struct sockaddr *)&server, sizeof(server))) != 0)
    {
        printf("TCP: Error in socket binding \n ");
        exit(0);
    }

    // listen to the socket connection
    if (listen(serverSocket, 3) != 0) // the no 5 is subjected to change. no of requests that can be queued
    {
        printf("TCP : Listening failed\n ");
        exit(0);
    }
    socklen_t clientLen = sizeof(client);

    // accept the incoming packet from client

    newSocket = accept(serverSocket, (struct sockaddr *)&client, &clientLen);

    if (newSocket < 0)
    {
        printf("TCP: Unable to accept the client packet\n ");
        exit(0);
    }

    char message[256];
    if ((recv(newSocket, message, sizeof(message), 0) < 0))
    {
            printf("TCP: Coundnt recieve file from client\n");
            exit(0);
    }
     fprintf(fp, "%s", message);
    // recieve message if any
    while (strlen(message)!=0)
    {
        printf("\n reading file");
        bzero(message, sizeof(message));
        if ((recv(newSocket, message, sizeof(message), 0) < 0))
        {
            printf("TCP: Coundnt recieve file from client\n");
            exit(0);
        }
        fprintf(fp, "%s", message);
        
       
    }

    printf("the file was received successfully");
    close(newSocket);
    close(serverSocket);
}
/**
 * @brief TCP client
 *
 * @param host
 * @param port
 * @param fp
 */

void tcp_client_ft(char *host, long port, FILE *fp)
{
    int clientSocket;
    struct sockaddr_in serveraddr;

    // create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        printf("TCP: Client socket creation failed \n");
        exit(1);
    }

    // resolve address

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    struct addrinfo *response;
    response = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    char str[256];
    sprintf(str, "%ld", port);
    getaddrinfo(host, str, &hints, &response);
    struct addrinfo *iterator = response;
    char buffer[4096];
    void *raw_addr;

    struct sockaddr_in *tmp = (struct sockaddr_in *)iterator->ai_addr;
    raw_addr = &(tmp->sin_addr);
    inet_ntop(AF_INET, raw_addr, buffer, 4096);

    // assign ip and port
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(buffer);
    serveraddr.sin_port = htons(port);

    // connect client socket with  server socket
    if (connect(clientSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
    {
        printf("TCP: Connection with server : %s  and port %s failed\n", buffer, str);
        exit(1);
    }
    char message[256]={0};

    while (fgets(message,256,fp) != NULL)
    {
        // send the file
        if (send(clientSocket, message, strlen(message), 0) < 0)
        {
            printf("TCP: Sending file from client failed\n");
            exit(1);
        }
    }
    printf("the file was sent successfully");
    close(clientSocket);
}

void udp_server_ft(char *iface, long port, FILE *fp)
{
}

void udp_client_ft(char *host, long port, FILE *fp)
{
}
