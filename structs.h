#ifndef STRUCTS_H
#define STRUCTS_H

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

typedef struct{
	vec2d pos;
	rgb32 col;
} player_info;

typedef struct{
	vec2d pos;
	vec2d dir;
	float radius;
	rgb32 col;
} ball_info;

typedef struct{
	player_info players[];
	ball_info ball;
} game_state;

#endif
