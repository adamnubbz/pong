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

//Global variables
#define NUMBER_OF_PLAYERS 2
#define PORT 8888

int running = 1;

fd_set readfds;
int max_clients = NUMBER_OF_PLAYERS;
int client_socket[NUMBER_OF_PLAYERS];

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
<<<<<<< HEAD
  //char message[1024];
	char* message;
=======
  char* message;
>>>>>>> bcc8031ac2569f705aa64f84dd874afd1c954bfd

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
          message = "Hello Client , I have received your connection. But I have to go now, bye\n";
          write(new_socket , message , strlen(message));
                 
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
            while(1){
							for(i = 0; i < NUMBER_OF_PLAYERS; i++){
								message = NULL;
								if(recv(client_socket[i], message, 2000, 0) < 0){
									puts("recv failed\n");
								} else {
									for(int j = 0; j < NUMBER_OF_PLAYERS; j++){
										if(send(client_socket[j], message, strlen(message), 0) < 0){
											puts("Send failed :(\n");
										}
									}
								}
								printf("message from player %d: %s\n", i, message);
							}//for
						}//while
          }
        }
    }

  sleep(3);
  //endwin();
  return 0;
}

