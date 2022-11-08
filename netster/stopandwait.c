#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
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

// meta data : file size that is going to sent through the RUDP channel
struct header
{
    long data_length;
};

void stopandwait_server(char *iface, long port, FILE *fp)
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        printf("UDP: Problem creating socket \n");
        exit(0);
    }
    printf("In server program\n");
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
        printf("UDP:  Error occured while receiving the message \n ");
        return;
    }
    printf("Recieved file length: %ld \n ", hdr.data_length);

    // receive data
    long count = 0;
    struct senderPacket recvd_packet;
    struct NackPacket nack;
    long prev = 0;
    char *filedata = (char *)malloc(sizeof(char) * bufferSize);

    printf("Entering the infinite loop:\n");
    for (;;)
    {
        // clear buffers
        memset(&recvd_packet, 0, sizeof(recvd_packet));
        memset(&nack, 0, sizeof(nack));
        bzero(filedata, bufferSize);
        printf("Recievd data :%ld  \n ", count);

        // if the entire file is recieved
        if (count == hdr.data_length)
        {
            break;
        }

        // recieve data from client
        int recivedbytes = recvfrom(serverSocket, (void *)(&recvd_packet), sizeof(recvd_packet), MSG_WAITALL, (struct sockaddr *)&client, &clientSize);
        // printf("\n Recieved %d bytes", recivedbytes);
        long seq = recvd_packet.seq;
        int data_length = recvd_packet.data_length;
        filedata = recvd_packet.payLoad;

        // printf("sequence noo: %ld\n", seq);
        // printf("size:%ld\n", data_length);
        // printf("Payload:%s\n", recvd_packet.payLoad);
        printf("Recived payload sixe: %d", sizeof(filedata));
        // if the payload is corrupted or recieve wasnt successfull
        if (recivedbytes < 0 || sizeof(filedata) != data_length)
        {
            printf("UDP hereeee : Error occured while receiving the message \n ");
            exit(1);
            // ask the sender to send the message again
            int dataSent = 0;
            while (dataSent <= 0)
            {
                printf("Here uh\n");
                nack.ack = -1;
                dataSent = sendto(serverSocket, (void *)(&nack), sizeof(nack), 0, (const struct sockaddr *)&client, clientSize);
            }
        }

        // check if the seq number is same as the last recieved packet
        else if (seq != 0 && seq == prev)
        {
            continue;
        }

        // write the data to file
        else
        {
            fwrite(filedata, sizeof(char), data_length, fp);
            fflush(fp);
            count += recivedbytes;
            int dataSent = 0;
            while (dataSent <= 0)
            {
                nack.ack = seq;
                dataSent = sendto(serverSocket, (void *)(&nack), sizeof(nack), 0, (const struct sockaddr *)&client, clientSize);
            }
        }

        prev = seq;
    }

    // printf("\n Total recieved:%d", count);
    free(response);

    close(serverSocket);
}

void stopandwait_client(char *host, long port, FILE *fp)
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

    // printf("In client program\n");

    // send filesize to server before sending the entire file content
    struct header hdr;
    hdr.data_length = filesize;

    int flag = sendto(clientSocket, (void *)(&hdr), sizeof(hdr), 0, (const struct sockaddr *)&server, serverSize);
    if (flag < 0)
    {
        printf("UDP:Unable to send file size to the server\n ");
        return;
    }
    printf("Sent file length: %ld \n", hdr.data_length);

    char *filedata = (char *)malloc(sizeof(char) * bufferSize);

    int count = 0; // chunks of data sent
    int seq = 0;   // sequence number
    struct senderPacket packet;
    struct NackPacket r_ack;
    printf("Entering the infite loop:\n");

    while (!feof(fp))
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
        // printf("Sending data : %s",packet.payLoad);
        // send the chunk of data read from the file to server
        for (;;)
        {
            
                int dataSent = sendto(clientSocket, (void *)(&packet), sizeof(packet), 0, (const struct sockaddr *)&server, serverSize);

                if (dataSent < 0)
                {
                    printf("UDP: Unable to send message to the server\n ");
                    exit(1);
                }
                int j = 0;
                int recivedbytes = 0;

                recivedbytes = recvfrom(clientSocket, (void *)(&r_ack), sizeof(r_ack), MSG_WAITALL, (struct sockaddr *)&server, &serverSize);

                while (j <= 50)
                {
                    j++;
                }
                if (recivedbytes < 0 || r_ack.ack != seq)
                {
                    continue;
                }
                else if (r_ack.ack == seq)
                {
                    break;
                }
            
        }

        count += ret;
        seq++;
    }
    // bzero(filedata, bufferSize);
    // printf("\n Total sent :%d", count);

    // if (sendto(clientSocket, filedata, 0, 0, (const struct sockaddr *)&server, serverSize) < 0)
    // {
    //     printf("UDP:Unable to send message to the server\n ");
    //     exit(1);
    // }
    free(filedata);
    free(response);
    // fflush(fp);
    close(clientSocket);
}
