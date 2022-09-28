#define BACKLOG 10
/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void chat_server(char* iface, long port, int use_udp) {

int sockfd, new_fd; // listen on sockfd, new connection on new_fd
struct sockaddr_in my_addr; // my address information
struct sockaddr_in their_addr; // client's address information
int sin_size;
if ((port = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
{
    perror("socket");
    exit(1);
}
my_addr.sin_family = AF_INET; // host byte order
my_addr.sin_port = htons(port); // short, network byte order
my_addr.sin_addr.s_addr = INADDR_ANY; // auto. filled with local IP
memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

if (bind(port, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) 
{
    perror("bind");
    exit(1);
}
if (listen(port, BACKLOG) == -1) 
{
    perror("listen");
    exit(1);
}
while(1)
{ // main accept() loop
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
    perror("accept");
    continue;
}
printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
{
    perror("send");
    close(new_fd);
  
}

void chat_client(char* host, long port, int use_udp) {
  
}

