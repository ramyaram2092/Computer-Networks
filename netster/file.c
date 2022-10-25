#include "file.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define bufferSize 256

void tcp_server_ft(char *iface, long port, FILE *fp);
void tcp_client_ft(char *host, long port, FILE *fp);
void udp_server_ft(char *iface, long port, FILE *fp);
void udp_client_ft(char *host, long port, FILE *fp);

const char *inet_ntop(int af, const void *restrict src,
                      char *restrict dst, socklen_t size);

struct header
{
    long data_length;
};

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

    struct header hdr;
    // receive file size
    if ((recv(newSocket, (void *)(&hdr), sizeof(hdr), 0) < 0))
    {
        printf("TCP: Coundnt recieve file from client\n");
        exit(0);
    }
    // printf("\n%ld", hdr.data_length);

    // resize filedata
    char *filedata = malloc(sizeof(char) * bufferSize);
    // receive data
    while (1)
    {
        bzero(filedata, bufferSize);
        int recivedbytes = recv(newSocket, filedata, bufferSize, 0);
        // printf("\n Recieved %d bytes", recivedbytes);

        if (recivedbytes < 0)
        {
            printf("TCP: Coundnt recieve file from client\n");
            exit(0);
        }
        else if (recivedbytes == 0)
        {
            break;
        }

        fwrite(filedata, sizeof(char), recivedbytes, fp);
    }
    free(filedata);
    fflush(fp);
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

    // find the file size
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // send filesize to server
    struct header hdr;
    hdr.data_length = filesize;
    send(clientSocket, (void *)(&hdr), sizeof(hdr), 0);

    char *filedata = (char *)malloc(sizeof(char) * bufferSize);

    // store read data into buffer
    while (!feof(fp))
    {
        bzero(filedata, bufferSize);
        size_t ret = fread(filedata, sizeof(char), bufferSize, fp);
        if (ret == 0)
        {
            fprintf(stderr, "fread() failed: %zu\n", ret);
            exit(1);
        }

        // send the file data to server

        if (send(clientSocket, filedata, ret, 0) < 0)
        {
            printf("TCP : Sending file from client failed \n");
            exit(1);
        }
        // printf("\n Sent %lu bytes", ret);
    }
    free(filedata);
    printf("the file was sent successfully");
    fflush(fp);
    close(clientSocket);
}

void udp_server_ft(char *iface, long port, FILE *fp)
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        printf("UDP: Problem creating socket \n");
        exit(0);
    }

    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    // resolve host name into ip  address

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
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(buffer);

    // bind socket with server
    if ((bind(serverSocket, (struct sockaddr *)&server, sizeof(server))) != 0)
    {
        printf("UDP : Error in Socket binding \n ");
        exit(0);
    }

    socklen_t clientSize = sizeof(client);

    struct header hdr;
    // receive file size
    int flag = recvfrom(serverSocket, (void *)(&hdr), sizeof(hdr), MSG_WAITALL, (struct sockaddr *)&client, &clientSize);
    if (flag < 0)
    {
        printf("UDP: Error occured while receiving the message \n ");
        return;
    }

    // resize filedata
    char *filedata = malloc(sizeof(char) * bufferSize);
    // receive data
    while (1)
    {
        bzero(filedata, bufferSize);
        int recivedbytes = recvfrom(serverSocket, filedata, bufferSize, MSG_WAITALL, (struct sockaddr *)&client, &clientSize);
        printf("\n Recieved %d bytes", recivedbytes);

        if (recivedbytes < 0)
        {
            printf("UDP: Error occured while receiving the message \n ");
            exit(0);
        }
        else if (recivedbytes == 0)
        {
            break;
        }

        fwrite(filedata, sizeof(char), recivedbytes, fp);
    }
    free(filedata);
    fflush(fp);
    close(serverSocket);
}

void udp_client_ft(char *host, long port, FILE *fp)
{
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        printf("UDP: Error in socket creation at client\n");
        return;
    }
    struct sockaddr_in server;
    socklen_t serverSize = sizeof(server);
    memset(&server, 0, sizeof(server));

    // resolve address

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
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

    // set server's ip and host
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(buffer);

    // find the file size
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // send filesize to server
    struct header hdr;
    hdr.data_length = filesize;

    int flag = sendto(clientSocket, (void *)(&hdr), sizeof(hdr), 0, (const struct sockaddr *)&server, serverSize);
    if (flag < 0)
    {
        printf("UDP:Unable to send message to the server\n ");
        return;
    }

    char *filedata = (char *)malloc(sizeof(char) * bufferSize);

    printf("\n Coming here");
    // store read data into buffer
    while (!feof(fp))
    {
            printf("\n Inside loop Coming here");


        bzero(filedata, bufferSize);
        size_t ret = fread(filedata, sizeof(char), bufferSize, fp);
        if (ret == 0)
        {
            fprintf(stderr, "fread() failed: %zu\n", ret);
            exit(1);
        }

        // send the file data to server

        if (sendto(clientSocket, filedata, ret, 0, (const struct sockaddr *)&server, serverSize) < 0)
        {
            printf("UDP:Unable to send message to the server\n ");
            exit(1);
        }
        printf("\n Sent %lu bytes", ret);
    }
    bzero(filedata, bufferSize);

    if (sendto(clientSocket, filedata, 0, 0, (const struct sockaddr *)&server, serverSize) < 0)
    {
        printf("UDP:Unable to send message to the server\n ");
        exit(1);
    }
    free(filedata);
    printf("the file was sent successfully gjh\n ");
    fflush(fp);
    close(clientSocket);
}
