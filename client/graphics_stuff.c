typedef struct{
  vec2d player1;
  vec2d player2;
  vec2d ball;
  vec2d ball_dir;
  //scores?
} game_state;

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
  
  // some connection stuff happens which we already have


  // Create a GUI window
  gui ui("Galaxy Simulation", WIDTH, HEIGHT);

  // need to generate paddles before or after generating bmp?
  
  // Render everything using this bitmap
  bitmap bmp(WIDTH, HEIGHT);

  while(running) {

    // send and receive structs that hold game state information (paddle location, ball location & direction, etc)

    // Update bmp based on received game_state information
    
    // Mark actions and update positions (button presses and paddles move)
    
    // Darken the bitmap instead of clearing it to leave trails
    bmp.darken(0.92);
    
    // Display the rendered frame
    ui.display(bmp);
  }
  return 0;
}//main
