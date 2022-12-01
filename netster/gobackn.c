#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>
#define bufferSize 256

// packet to send
struct senderPacket
{
    long seq;
    int data_length;
    char payLoad[bufferSize];
};

// recieved packet acknowledgement
struct NackPacket
{
    long ack;
};

// meta data : file size that is going to be sent through the RUDP channel
struct header
{
    int seq;
    long data_length;
};

// node definition
struct node
{
    struct senderPacket pk;
    struct node *next;
};

/* Add function definitions */
void gbn_server(char *iface, long port, FILE *fp)
{
}

void gbn_client(char *host, long port, FILE *fp)
{

    // client socket creation
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        perror("UDP: Error in socket creation at client\n");
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

    // set the timeout for socket
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 70000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    // send filesize to server before sending the entire file content

    struct header hdr;
    hdr.data_length = filesize;
    int count = 0; // chunks of data sent
    int seq = 0;   // sequence number
    struct NackPacket r_ack;
    for (;;)
    {
        int flag = sendto(clientSocket, (void *)(&hdr), sizeof(hdr), 0, (const struct sockaddr *)&server, serverSize);
        if (flag < 0)
        {
            perror("UDP:Unable to send file size to the server\n ");
            exit(0);
        }
        int recivedbytes = 0;
        recivedbytes = recvfrom(clientSocket, (void *)(&r_ack), sizeof(r_ack), MSG_WAITALL, (struct sockaddr *)&server, &serverSize);

        // ack not recieved or timeout
        if (recivedbytes < 0 || r_ack.ack != seq)
        {
            printf("Waiting time exceeded / Incorrect ack . Resending the data\n ");
            continue;
        }
        // ack recieved
        else if (r_ack.ack == seq)
        {
            break;
        }
    }

    // Logic to send the file
    char *filedata = (char *)malloc(sizeof(char) * bufferSize);
    count = 0, seq = 0;
    struct senderPacket packet;

    // printf("ENTERING THE INFINITE LOOP \n");
    int windowsize = 5;

    struct node *head = (struct node *)malloc(sizeof(struct node));
    struct node *current = head;

    while (!feof(fp))
    {

        int i = 0;

        // prepare  5 packets
        while (i < windowsize)
        {
            // clear buffers
            memset(&packet, 0, sizeof(packet));
            memset(&r_ack, 0, sizeof(r_ack));
            bzero(filedata, bufferSize);
            r_ack.ack = -1;

            // read data from file and store it in filedata
            int ret = fread(filedata, sizeof(char), bufferSize, fp);
            if (ret == 0)
            {
                fprintf(stderr, "fread() failed: %d\n", ret);
                exit(1);
            }

            // set the packet contents

            packet.data_length = ret;

            int k = 0;
            while (k < ret)
            {
                packet.payLoad[k] = filedata[k];
                k++;
            }
            packet.seq = seq;

            // put the packet in linked list
            current->pk = packet;
            current->next = (struct node *)malloc(sizeof(struct node));
            current = current->next;
            i++;
        }

        // send those 5 packets
        while (current->next != NULL)
        {
            // sending the chunk of data read from the file to server

            int dataSent = sendto(clientSocket, (void *)(&packet), sizeof(packet), 0, (const struct sockaddr *)&server, serverSize);

            if (dataSent < 0)
            {
                perror("UDP: Unable to send message to the server\n ");
                exit(1);
            }
            current=current->next;
            seq++;
        }

        // check if acknowledgment has been recieved for all the files
        current= head;

        while(current->next!=NULL)
        {

            int recivedbytes = 0;

            // printf("Waiting for acknowledgement from reciever\n");
            recivedbytes = recvfrom(clientSocket, (void *)(&r_ack), sizeof(r_ack), MSG_WAITALL, (struct sockaddr *)&server, &serverSize);

            if (recivedbytes < 0 || r_ack.ack != seq)
            {
                head=current;
                break;
            }
            else if (r_ack.ack == seq)
            {
                // printf("Recieved acknowledgment\n");
                continue;
            }
        }

    }

    free(filedata);
    free(response);
    close(clientSocket);
}
