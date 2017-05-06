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
  
void* read_sockets(void* args){
  char server_reply[1024];
  int* socket = (int*) args;
  //Receive a reply from the server
  while(1){
    if(recv(*socket, server_reply , 1024, 0) < 0){
      puts("recv failed");
    } else {
      puts(server_reply);
      memset(server_reply, 0, 1024);
    }
  }
}

void* write_sockets(void* args){
  char message[1024];
  int* socket = (int*) args;
  //Send some data
  while(1){
    if(fgets(message, 100, stdin) != NULL){
      if(send(*socket, message , 1024*sizeof(message), 0) < 0){
        perror("Send failed");
      }
      else {
        memset(message, 0, 1024);
      }
    }
    if (strcmp(message, "exit") == 0){
      break;
    }
  }
}

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

  pthread_t threads[2];

  if(pthread_create(&threads[0], NULL, read_sockets, &socket_desc) != 0){
    perror("Error creating thread 1");
    exit(2);
  }

  if(pthread_create(&threads[1], NULL, write_sockets, &socket_desc) != 0){
    perror("Error creating thread 2");
    exit(2);
  }

  if(pthread_join(threads[0], NULL) != 0) {
    perror("Error joining with thread 1");
    exit(2);
  }

  if(pthread_join(threads[1], NULL) != 0) {
    perror("Error joining with thread 1");
    exit(2);
  }  

  return 0;
}
