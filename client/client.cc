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
#include "../structs.hh"
#include <pthread.h>

#include <SDL.h>

#include "bitmap.hh"
#include "gui.hh"
#include "util.hh"
#include "vec2d.hh"

typedef struct{
	int socket;
	char message[3];
} thread_args;

// Initial velocity
int x_vel = 10;
int y_vel = 15;

// Paddle size
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50

// Screen size
#define WIDTH 800
#define HEIGHT 600

void initGame(game_state* game);
void drawGame(bitmap* bmp, game_state* game);
char read_input(int socket);
void* read_sockets(void* args);
void* write_sockets(void* args);
void initScreen(bitmap* bmp);


game_state* game = (game_state*) malloc(sizeof(game_state));
rgb32 gray = {100, 100, 100};
bool running = true;
pthread_mutex_t lock;

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
  
  pthread_mutex_init(&lock, NULL);
  
  initGame(game);
        
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

  // Create a GUI window
  gui ui("Pong Game!", WIDTH, HEIGHT);
  
  // Render everything using this bitmap
  bitmap bmp(WIDTH, HEIGHT);

  pthread_t threads[2];
  thread_args* t_args = (thread_args*) malloc(sizeof(thread_args));
  t_args->socket = socket_desc;
  //assign message here

  printf("%d\n", socket_desc);
  if(pthread_create(&threads[1], NULL, write_sockets, t_args) != 0){
    perror("Error creating thread 2");
    exit(2);
  }

  if(pthread_create(&threads[0], NULL, read_sockets, &socket_desc) != 0){
    perror("Error creating thread 1");
    exit(2);
  }

  initScreen(&bmp);
  
  while(running) {
    // Process events
    SDL_Event event;
    while(SDL_PollEvent(&event) == 1) {
      // If the event is a quit event, then leave the loop
      if(event.type == SDL_QUIT){
        running = false;
      }
    }
    
    // Get the keyboard state
    const uint8_t* keyboard = SDL_GetKeyboardState(NULL);

    FD_ZERO(&readfds);

    //Add stdin
    FD_SET(stdin_sd, &readfds);
    max_sd = stdin_sd;

    FD_SET(socket_desc, &readfds);
    if(socket_desc > max_sd)
      max_sd = socket_desc;

    // Update bmp based on received game_state information
    drawGame(&bmp, game);
    
    // Darken the bitmap instead of clearing it to leave trails
    bmp.darken(0.92);
    
    // Display the rendered frame
    ui.display(bmp);
  }

  return 0;
}



void* read_sockets(void* args){
  game_state server_reply;
  int* socket = (int*) args;
  //Receive a reply from the server
  while(1){
    if(recv(*socket, &server_reply , sizeof(game_state), 0) < 0){
      puts("recv failed");
    } else {
      pthread_mutex_lock(&lock);
      memcpy(game, &server_reply, sizeof(game_state));
      pthread_mutex_unlock(&lock);
    }
  }
}

void* write_sockets(void* args){
  int socket = ((thread_args*)args)->socket;
  char* message = ((thread_args*)args)->message;
  //Send some data
  while(running){
    
    // Get the keyboard state
    const uint8_t* keyboard = SDL_GetKeyboardState(NULL);
    
    // If the up key is pressed, shift up one pixel
    if(keyboard[SDL_SCANCODE_UP]) {
      message[0] = 'w';
      if(send(socket, message , 3*sizeof(char), 0) < 0){
        printf("%d\n", socket);
        perror("Send failed");
      }
    }
    
    // If the down key is pressed, shift down one pixel
    if(keyboard[SDL_SCANCODE_DOWN]) {
      message[0] = 's';
      if(send(socket, message , 3*sizeof(char), 0) < 0){
        printf("%d\n", socket);
        perror("Send failed");
      }
    }

  }
  return NULL;
}

void drawGame(bitmap* bmp, game_state* game) {
  
  // Draw the paddles
  for(int i = 0; i < 2; i++){
    float x_coord = game->players[i].pos.x();
    float max_x_coord = x_coord + PADDLE_WIDTH;
    for(; x_coord < max_x_coord; x_coord++){
      float y_coord = game->players[i].pos.y();
      float max_y_coord = y_coord + PADDLE_HEIGHT;
      for(; y_coord < max_y_coord; y_coord++){
        bmp->set(x_coord, y_coord, game->players[i].color);
      }
    }
  }
  
  // Draw the ball
  float center_x = game->ball.pos.x();
  float center_y = game->ball.pos.y();
  float radius = game->ball.radius;
  rgb32 color = game->ball.color;
  
  // Loop over points in the upper-right quadrant of the circle
  for(float x = 0; x <= radius*1.1; x++) {
    for(float y = 0; y <= radius*1.1; y++) {
      // Is this point within the circle's radius?
      float dist = sqrt(pow(x, 2) + pow(y, 2));
      if(dist < radius) {
        // Set this point, along with the mirrored points in the other three quadrants
        bmp->set(center_x + x, center_y + y, color);
        bmp->set(center_x + x, center_y - y, color);
        bmp->set(center_x - x, center_y - y, color);
        bmp->set(center_x - x, center_y + y, color);
      }
    }
  }
  
}

void initGame(game_state* game){
  game->players[0].pos = vec2d(50, 350);
  game->players[0].color = {255, 50, 50};
  game->players[1].pos = vec2d(740, 350);
  game->players[1].color = {50, 50, 255};
  game->ball.radius = 5.0;
  game->ball.pos = vec2d(400,300);
  game->ball.color = {255, 255, 255};
}
  
void initScreen(bitmap* bmp){
  for(float x = 0; x < WIDTH; x++){
    for(float y = 0; y < HEIGHT; y++){
        bmp->set(x, y, {0, 0, 0});
    }
  }

  // Draw the borders
  for(float y = 50; y < 70; y++){
    for(float x = 50; x < 750; x++){
      bmp->set(x, y, gray);
      bmp->set(x, (y + 475), gray);
    }
  }
}
