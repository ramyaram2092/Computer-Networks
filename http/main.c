#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void send_http(char* host, char* msg, char* resp, size_t len);


/*
  Implement a program that takes a host, verb, and path and
  prints the contents of the response from the request
  represented by that request.
 */
int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Invalid arguments - %s <host> <GET|POST> <path>\n", argv[0]);
    return -1;
  }
  char* host = argv[1];
  char* verb = argv[2];
  char* path = argv[3];

  /*
    STUDENT CODE HERE
   */
  char* header=" HTTP/1.0\r\n";
  char* message=(char*) malloc(strlen(verb)+strlen(path)+strlen(header)+strlen(host)+10);
  strcpy(message,verb);
  strcat(message," ");
  strcat(message,path);
  strcat(message,header);
  strcat(message,"Host:");
  strcat(message,host);
  strcat(message,"\r\n\r\n");

  printf("\n Host is %s",host);
  printf("\n Message is %s", message); 
  char response[4096];
  send_http(host,message,response,4096);
  printf("%s\n", response);
  return 0;
}
