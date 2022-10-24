#include "file.h"
#include <stdio.h>
void tcp_server_ft(char *iface, long port, FILE *fp);
void tcp_client_ft(char *host, long port, FILE *fp);
void udp_server_ft(char *iface, long port, FILE *fp);
void udp_client_ft(char *host, long port, FILE *fp);

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

    int i = 0;
}

void tcp_client_ft(char *host, long port, FILE *fp)
{
}

void udp_server_ft(char *iface, long port, FILE *fp)
{
}

void udp_client_ft(char *host, long port, FILE *fp)
{
}
