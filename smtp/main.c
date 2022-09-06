#include <stdio.h>
#include <string.h>
#include<stdlib.h>
int connect_smtp(const char* host, int port);
void send_smtp(int sock, const char* msg, char* resp, size_t len);



/*
  Use the provided 'connect_smtp' and 'send_smtp' functions
  to connect to the "lunar.open.sice.indian.edu" smtp relay
  and send the commands to write emails as described in the
  assignment wiki.
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid arguments - %s <email-to> <email-filepath>", argv[0]);
    return -1;
  }

  char* rcpt = argv[1];
  char* filepath = argv[2];

  /* 
     STUDENT CODE HERE
   */
  
   char *mailfrom= "mail from: <";
   char *delimiter="> \n";
   char *mailfrom_cmd=(char* )malloc(strlen(mailfrom)+strlen(delimiter)+strlen(rcpt)+10);
   strcpy(mailfrom_cmd,mailfrom);
   strcat(mailfrom_cmd,rcpt);
   strcat(mailfrom_cmd,delimiter);
   

   char *recptTo="rcpt to:<";
   char *recptTo_cmd=(char* )malloc(strlen(recptTo)+strlen(delimiter)+strlen(rcpt)+10);
   strcpy(recptTo_cmd,recptTo);
   strcat(recptTo_cmd,rcpt);
   strcat(recptTo_cmd,delimiter);

   FILE *fp;
   fp=fopen(filepath,"r");
   char buff[1000];
   fgets(buff,1000,fp);
   strcat(buff,"\r\n.\r\n");

   
   printf(" RCPT is %s",rcpt);
   printf(" Filepath is %s",filepath);
   int socket= connect_smtp("lunar.open.sice.indiana.edu",25);
   char response[4096];
   send_smtp(socket,"HELO iu.edu \n",response,4096);
   printf("HELO Response is %s",response);
   send_smtp(socket,mailfrom_cmd,response,4096);
   printf(" MAIL FROM Response is %s",response);
   send_smtp(socket,recptTo_cmd,response,4096);
   printf("RCPT TO response is %s",response);
   send_smtp(socket,"DATA \n",response,4096);
   printf("DATA response is %s",response);
   send_smtp(socket,buff,response,4096);
      fclose(fp);

   printf("final response is %s",response);




  
  return 0;
}
