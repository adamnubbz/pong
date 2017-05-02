//Select and pull to handle simulataneous messaging
#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>

//main
int main(int argc , char *argv[])
{
  //Variables
  int socket_desc;
  struct sockaddr_in server;
  char message[1024];
  char server_reply[1024];
  int max_sd;
  int server_sd, activity;
  int stdin_sd = fileno(stdin);
  fd_set readfds;
        
  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket");
    }

  //Read ip and connect to it
  char server_ip[1024];
  printf("Please enter IP address to connect: ");
  scanf("%s", server_ip);
    
  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_family = AF_INET;
  server.sin_port = htons( 8888 );
 
  //Connect to remote server
  if ((server_sd = connect(socket_desc , (struct sockaddr *)&server , sizeof(server))) < 0)
    {
      puts("connect error");
      return 1;
    }
    
  puts("Connected\n");

  //Send some data
 	while(1){
    if(fgets(message, 100, stdin) != NULL && send(socket_desc , message , strlen(message) , 0) < 0){
    	puts("Send failed");
    	return 1;
    } else {
			message[strlen(message)] = '\0';
    	puts("Data Send\n");
			printf("Message = %s\n", message);
		}
		if (strcmp(message, "exit") == 0){
			return 0;
		}
		//Receive a reply from the server
    if(recv(socket_desc, server_reply , 2000 , 0) < 0){
    	puts("recv failed");
    } else {
    	puts("Reply received\n");
    	puts(server_reply);
		}
  }

  return 0;
}
