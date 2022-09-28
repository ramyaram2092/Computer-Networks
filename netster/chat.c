#define BACKLOG 10
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
    int socket_fd, new_fd; // listen on sockfd, new connection on new_fd
    struct sockaddr_in server_addr; // my address information
    struct sockaddr_in client_addr; // client's address information

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET; // host byte order
    server_addr.sin_port = htons(port); // short, network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // auto. filled with local IP
    memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
    {
        perror("bind");
        exit(1);
    }
    printf("\n Hello, I am a server");

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
    
}


















// socket(AF_INET, SOCK_STREAM, 0)) creates a socket connection  in the port we are listening 
// MYPORT is the port value that we are recieving in the function call


// #define MYPORT 3490 // the port users will be connecting to
// #define BACKLOG 10 // how many pending connections queue will hold
// int main(void) {
// int sockfd, new_fd; // listen on sockfd, new connection on new_fd
// struct sockaddr_in server_addr; // my address information
// struct sockaddr_in their_addr; // connector's address information
// int sin_size;
// if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
// perror("socket");
// exit(1);
// }
// server_addr.sin_family = AF_INET; // host byte order
// server_addr.sin_port = htons(MYPORT); // short, network byte order
// server_addr.sin_addr.s_addr = INADDR_ANY; // auto. filled with local IP
// memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct


// if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
// perror("bind");
// exit(1);
// }
// if (listen(sockfd, BACKLOG) == -1) {
// perror("listen");
// exit(1);
// }
// while(1) { // main accept() loop
// sin_size = sizeof(struct sockaddr_in);
// if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
// perror("accept");
// continue;
// }
// printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
// if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
// perror("send");
// close(new_fd);
// }
// return