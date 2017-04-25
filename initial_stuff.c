

//Global variables
#define NUMBER_OF_PLAYERS 4

fd_set readfds;
int max_clients = NUMBER_OF_PLAYERS;
int client_socket[NUMBER_OF_PLAYERS];

int main(int argc, char** argv)
{

  int master_socket;
  
  struct sockaddr_in address;
  
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
      
    }

  
  return 0;
}
