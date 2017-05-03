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
  } else if(key == KEY_RIGHT) {
    key_ts = 'd';
    is_valid_key = 1;
  } else if(key == KEY_DOWN) {
    key_ts = 's';
    is_valid_key = 1;
  } else if(key == KEY_LEFT) {
    key_ts = 'a';
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
