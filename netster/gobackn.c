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
// #define DEBUGLOGS

#ifdef DEBUGLOGS
#define DEBUGMSG(...) printf(__VA_ARGS__)
#else
#define DEBUGMSG(...)
#endif
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

// Function declarations
struct node *constructLinkedList(FILE *fp, int *totalpackets);

/* Add function definitions */
void gbn_server(char *iface, long port, FILE *fp)
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        perror("UDP: Problem creating socket \n");
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
    //

    // bind socket with server
    if ((bind(serverSocket, (struct sockaddr *)&server, sizeof(server))) != 0)
    {
        perror("UDP : Error in Socket binding \n ");
        exit(0);
    }

    socklen_t clientSize = sizeof(client);

    struct header hdr;
    long count = 0;
    struct senderPacket recvd_packet;
    struct NackPacket nack;

    // receive  the file size first
    DEBUGMSG("WAITING FOR FILE SIZE \n");
    for (;;)
    {
        int flag = recvfrom(serverSocket, (void *)(&hdr), sizeof(hdr), MSG_WAITALL, (struct sockaddr *)&client, &clientSize);
        if (flag < 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    int dataSent = 0;

    DEBUGMSG("SENDING ACKNOWLEDGEMENT \n");

    while (dataSent <= 0)
    {
        nack.ack = hdr.seq;
        dataSent = sendto(serverSocket, (void *)(&nack), sizeof(nack), 0, (const struct sockaddr *)&client, clientSize);
    }
    DEBUGMSG("Length of the file to be recieved: %ld \n ", hdr.data_length);

    // receive  the actual data

    long prev = 0;
    char *filedata = (char *)malloc(sizeof(char) * bufferSize);

    DEBUGMSG(" RECIEVING ACTUAL FILE DATA \n");
    for (;;)
    {
        // clear buffers
        memset(&recvd_packet, 0, sizeof(recvd_packet));
        memset(&nack, 0, sizeof(nack));
        bzero(filedata, bufferSize);
        DEBUGMSG("Data recieved so far :%ld  \n ", count);

        // if the entire file is recieved
        if (count == hdr.data_length)
        {
            DEBUGMSG("RECEIVED ALL DATA NEEDED TO BE RECIEVED\n");
            break;
        }

        // recieve data from client
        int recivedbytes = recvfrom(serverSocket, (void *)(&recvd_packet), sizeof(recvd_packet), MSG_WAITALL, (struct sockaddr *)&client, &clientSize);
        long seq = recvd_packet.seq;
        int data_length = recvd_packet.data_length;
        filedata = recvd_packet.payLoad;

        int payloadSize = recivedbytes - 2 * (sizeof(long));
        DEBUGMSG("Expected payload size: %d\n", data_length);
        DEBUGMSG("Recived payload size: %d with seq number %ld\n", payloadSize, seq);
        // int flag =-1;

        // if the payload is corrupted or recieve wasnt successfull ask the sender to send the message again  or if the seq number is same as the last recieved packet
        if (recivedbytes < 0 || payloadSize < data_length || seq != prev + 1)
        {
            DEBUGMSG("Skipping the  sequence no %ld  as it is redundant or not in order or data is corrupted \n", seq);
            nack.ack = prev;
             
        }

        // write the data to file
        else
        {
            DEBUGMSG("WRITING DATA TO THE FILE\n");
            fwrite(filedata, sizeof(char), data_length, fp);
            fflush(fp);
            count += data_length;
            nack.ack = seq;
            prev = seq;
        }
        int dataSent = 0;
        while (dataSent <= 0)
        {
            DEBUGMSG("SENDING ACKNOWLEDGEMENT  for packet with sequence no %ld\n", seq);
            dataSent = sendto(serverSocket, (void *)(&nack), sizeof(nack), 0, (const struct sockaddr *)&client, clientSize);
        }
    }

    DEBUGMSG("\nTOTAL DATA RECIEVED :%ld", count);
    free(response);
    fflush(stdout);

    close(serverSocket);
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
    read_timeout.tv_usec = 1000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    // send filesize to server before sending the entire file content
    //
    struct header hdr;
    hdr.data_length = filesize;
    // int count = 0; // chunks of data sent
    int seq = 0; // sequence number
    struct NackPacket r_ack;

    DEBUGMSG("SENDING FILE SIZE TO RECIEVER \n");
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
            DEBUGMSG("Waiting time exceeded/Incorrect Ack . Resending the data\n");
            continue;
        }
        // ack recieved
        else if (r_ack.ack == seq)
        {
            break;
        }
    }

    // Logic to send the file
    struct senderPacket packet;

    DEBUGMSG("FILE TRANSFER BEGINS \n");
    int windowsize = 1;
    int totalpackets = 0;
    struct node *head = constructLinkedList(fp, &totalpackets);
    int i = 0;
    while (i < totalpackets)
    {
        struct node *current = head;
        int j = 1;
        // send 5 packets
        DEBUGMSG("SENDING %d PACKETS \n", windowsize);
        DEBUGMSG("starting to send from packet %d \n", i + j);
        while (j <= windowsize && (i + j) <= totalpackets)
        {
            memset(&packet, 0, sizeof(packet));
            packet = current->pk;

            // sending the chunk of data read from the file to

            int dataSent = sendto(clientSocket, (void *)(&packet), sizeof(packet), 0, (const struct sockaddr *)&server, serverSize);

            if (dataSent < 0)
            {
                perror("UDP: Unable to send message to the server\n");
                exit(1);
            }
            current = current->next;
            j++;
            DEBUGMSG("Sent %d bytes of data  with sequence number %ld \n", dataSent, packet.seq);
        }
        int sent = j - 1;

        // check if acknowledgment has been recieved for all the files
        current = head;
        DEBUGMSG(" WAITING FOR ACKNOWLEDGEMENT FROM RECIEVER \n");
        j = 1;
        while (j <= windowsize && (i + j) <= totalpackets)
        {
            memset(&r_ack, 0, sizeof(r_ack));
            memset(&packet, 0, sizeof(packet));
            packet = current->pk;

            int recivedbytes = 0;

            recivedbytes = recvfrom(clientSocket, (void *)(&r_ack), sizeof(r_ack), MSG_WAITALL, (struct sockaddr *)&server, &serverSize);

            if (recivedbytes < 0 || r_ack.ack != packet.seq)
            {
                DEBUGMSG("OOPSSS didnt recieve acknowledgment for the packet with  seq number %ld\n", packet.seq);
                // if the seq number received is greater than the current pack seq number
                while (current->pk.seq <= r_ack.ack)
                {
                    current = current->next;
                }
                head = current;
                break;
            }
            else if (r_ack.ack == packet.seq)
            {
                head = current->next;
                DEBUGMSG("RECEIVED acknowledgment for the packet with  seq number %ld\n", packet.seq);
                i++;
            }
            current = current->next;
            j++;
        }
        int ackrecvd = j - 1;
        if (sent == ackrecvd)
        {
            windowsize *= 2;
        }
        else
        {
            windowsize -= 1;
        }
    }

    free(response);
    fflush(stdout);
    close(clientSocket);
}

/**
 * @brief The following function constructs the packets and adds it to a linked list
 *
 * @param fp : file pointer
 * @param totalpackets : a pointer variable which needs to be updated with total number of packets value
 * @return struct node* : return the head pointer to the linked list
 */
struct node *constructLinkedList(FILE *fp, int *totalpackets)
{
    char *filedata = (char *)malloc(sizeof(char) * bufferSize);
    struct senderPacket packet;
    int seq = 1;
    struct node *head = (struct node *)malloc(sizeof(struct node));
    head->next = NULL;
    struct node *current = head;
    DEBUGMSG("CONSTRUCTING PACKETS AND ADDING THEM TO THE LIST\n");
    while (!feof(fp))
    {

        // clear buffers
        memset(&packet, 0, sizeof(packet));
        bzero(filedata, bufferSize);

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
        current->next = NULL;
        seq++;
        DEBUGMSG("Packed %d bytes of data \n", ret);
    }
    *totalpackets = seq - 1;
    DEBUGMSG("THE NUMBER OF PACKETS CONSTRUCTED FOR THIS FILE IS %d\n", seq - 1);
    free(filedata);

    return head;
}
