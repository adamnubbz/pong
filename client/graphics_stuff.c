typedef struct{
  vec2d pos;
  rgb32 col;
  //bool vert // if 4 players, vert determines whether paddle is vert or hori
} player_info;

typedef struct{
  vec2d pos;
  vec2d dir;
  float radius;
  rgb32 col;
} ball_info

typedef struct{
  player_info players[];
  ball_info ball;
  //scores?
} game_state;

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50

//main
int main(int argc , char *argv[])
{
  //Variables
  int socket_desc;
  struct sockaddr_in server;
  char message[1024];
  int max_sd;
  int server_sd, activity;
  int stdin_sd = fileno(stdin);
  fd_set readfds;
  game_state* game = (game_state*) malloc(sizeof(game_state));
  
  // some connection stuff happens which we already have


  // Create a GUI window
  gui ui("Galaxy Simulation", WIDTH, HEIGHT);

  // need to generate paddles before or after generating bmp?
  
  // Render everything using this bitmap
  bitmap bmp(WIDTH, HEIGHT);

  while(running) {

    FD_ZERO(&readfds);
      
    //Add stdin
    FD_SET(stdin_sd, &readfds);
    max_sd = stdin_sd;

    FD_SET(socket_desc, &readfds);
    if(socket_desc > max_sd)
      max_sd = socket_desc;

     //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

    //error check for select
    if ((activity < 0) && (errno!=EINTR)) 
      {
        printf("select error");
      }
    
    //Check for keystrokes and send to server
    char keystroke = read_input(socket_desc);
    
    if(keystroke != -50){
      char keystroke_ts[3];
      keystroke_ts[0] = keystroke;            
      keystroke_ts[1] = '\n';
      keystroke_ts[2] = '\0';
      send(socket_desc, keystroke_ts, strlen(keystroke_ts) , 0);
    }
    // send keystroke information to server, receive updated game_state from
    // server (server does the updating)

    // Charlie, how do I receive a game_state from the server? Aka, how do
    // I receive the array, parse it, and update the game_state for the client

    // will probably need something of the form:
    // updateGame(receivedmessage, game);

    // Update bmp based on received game_state information
    drawPaddles(bmp, game);
    
    // Mark actions and update positions (button presses and paddles move)
    
    // Darken the bitmap instead of clearing it to leave trails
    bmp.darken(0.92);
    
    // Display the rendered frame
    ui.display(bmp);
  }
  return 0;
}//main




//Reads key pressed and returns it as char
//If no key was pressed, or if a key was pressed that wasn't an arrow key, function
//returns -50;
char read_input(int socket) {
  int key = getch();
  char key_ts;
  int is_valid_key = 0;
  if(key == KEY_UP) {
    key_ts = 'w';
    is_valid_key = 1;
  } else if(key == KEY_DOWN) {
    key_ts = 's';
    is_valid_key = 1;
  } else if(key == 'q') {
    key_ts = 'q';
    is_valid_key = 1;
  }

  if(is_valid_key == 1){
    return key_ts;
  }
  else
    return -50;
}//read_input


void drawGame(bitmap* bmp, game_state* game) {

  // Draw the paddles
  int i = 0;
  for(i; i < 2; i++){
    rgb32 color = game.players[i].col;
    float x_coord = game.players[i].pos.x();
    float max_x_coord = x_coord + PADDLE_WIDTH;
    for(x_coord; x_coord < max_x_coord; x_coord++){
      float y_coord = game.players[i].pos.y();
      float max_y_coord = y_coord + PADDLE_HEIGHT;
      for(y_coord; y_coord < max_y_coord; y_coord++){
        bmp->set(x_coord, y_coord, color);
      }
    }
  }
  
  // Draw the ball
  float center_x = game.ball.pos.x();
  float center_y = game.ball.pos.y();
  float radius = game.ball.radius();
  rgb32 color = game.ball.col;
  
  // Loop over points in the upper-right quadrant of the circle
  for(float x = 0; x <= radius*1.1; x++) {
    for(float y = 0; y <= radius*1.1; y++) {
      // Is this point within the circle's radius?
      float dist = sqrt(pow(x, 2) + pow(y, 2));
      if(dist < radius) {
        // Set this point, along with the mirrored points in the other three quadrants
        bmp->set(center_x + x + x_offset, center_y + y + y_offset, color);
        bmp->set(center_x + x + x_offset, center_y - y + y_offset, color);
        bmp->set(center_x - x + x_offset, center_y - y + y_offset, color);
        bmp->set(center_x - x + x_offset, center_y + y + y_offset, color);
      }
    }
  }
}
  
