/*Program that runs the server for the game*/

#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <curses.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h> //hostent
#include <arpa/inet.h> // IP address
#include <pthread.h>

//Global variables
#define NUMBER_OF_PLAYERS 2
#define PORT 8888

int running = 1;

fd_set readfds;
int max_clients = NUMBER_OF_PLAYERS;
int client_socket[NUMBER_OF_PLAYERS];

typedef struct thread_args {
  int* sockets;
  int index;
} thread_args_t;

void* read_sockets(void* args){
  char server_reply[1024];
  int* sockets = ((thread_args_t*) args)->sockets;
  int index = ((thread_args_t*) args)->index; 

  //Receive a reply from the server
  while(1){
    if(recv(sockets[index], server_reply , 1024, 0) < 0){
      puts("recv failed");
    } else {
      for(int i = 0; i < NUMBER_OF_PLAYERS; i++){
        //send to all sockets that the message was not received from
        if(i != index){
          if(send(sockets[i], server_reply, 1024, 0) < 0){
            perror("Send failed");
          }
        }
      }
      memset(server_reply, 0, 1024);
    }
  }
}

int main(int argc, char** argv)
{
  int opt = TRUE;
  int master_socket, addrlen, new_socket;
  int activity, i, valread, sd;
  int max_sd;
  int sd_last;
  int has_stdin = 0;
  int client_sock;
  int connections = 0;
  
  struct sockaddr_in address;
  char message[2000];

  //create a master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

  //set master socket to allow multiple connections
  if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  //bind the socket to localhost port 8888
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
  printf("Listener on port %d \n", PORT);
  //printf("%s is the address.\n", inet_ntoa(address));

  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, NUMBER_OF_PLAYERS + 1) < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while(running) 
    {
      //clear the socket set
      FD_ZERO(&readfds);

      FD_SET(master_socket, &readfds);
      max_sd = master_socket;
      
      FD_SET(fileno(stdin), &readfds);

      //add sockets to readfds
      for ( i = 0 ; i < max_clients ; i++) 
        {
          //socket descriptor
          sd = client_socket[i];
             
          //if valid socket descriptor then add to read list
          if(sd > 0)
            FD_SET( sd , &readfds);
             
          //highest file descriptor number, need it for the select function
          if(sd > max_sd)
            max_sd = sd;
        }

      //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
      activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
      if ((activity < 0) && (errno!=EINTR)) 
        {
          printf("select error");
        }
          
      //If something happened on the master socket , then its an incoming connection
      if (FD_ISSET(master_socket, &readfds)) 
        {
          if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
              perror("accept");
              exit(EXIT_FAILURE);
            }

         
          //inform user of socket number - used in send and receive commands
          printf("Player connected , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
          connections++;
          //Reply to the client
          char* greeting = "Hello Client , I have received your connection. But I have to go now, bye\n";
          send(new_socket , greeting , strlen(greeting), 0);
                 
          //add new socket to array of sockets
          for (i = 0; i < max_clients; i++) 
            {
              //if position is empty
              if( client_socket[i] == 0 )
                {
                  client_socket[i] = new_socket;
                  if (connections < NUMBER_OF_PLAYERS)
                    printf("Waiting for Player 2 to connect...\n");            
                  break;
                }
            }
          //If we have right number of players, initialize game by initializing scheduler
          if (connections == NUMBER_OF_PLAYERS){
            //scheduler_init();//start scheduler
            printf("Hello, game start!\n");
            pthread_t threads[NUMBER_OF_PLAYERS];
            thread_args_t args[NUMBER_OF_PLAYERS];
            for(int j = 0; j < NUMBER_OF_PLAYERS; j++){
              args[j].sockets = client_socket;
              args[j].index = j;
              if(pthread_create(&threads[j], NULL, read_sockets, &args[j]) != 0){
                perror("Error creating thread 1");
                exit(2);
              }
            }
            for(int j = 0; j < NUMBER_OF_PLAYERS; j++){
              if(pthread_join(threads[j], NULL) != 0) {
                perror("Error joining with thread");
                exit(2);
              }
            } 
          }
        }
    }

  sleep(3);
  //endwin();
  return 0;
}

