#ifndef STRUCTS_HH
#define STRUCTS_HH

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

#include "client/vec2d.hh"
#include "client/bitmap.hh"

typedef struct{
	vec2d pos;
	rgb32 color;
} player_info;

typedef struct{
	vec2d pos;
	vec2d dir;
	float radius;
	rgb32 color;
} ball_info;

typedef struct{
	player_info players[2];
	ball_info ball;
} game_state;

#endif
