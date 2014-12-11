#ifndef __MAIN
#define __MAIN

#define NO_STDIO_REDIRECT

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <SDL_thread.h>

#ifdef __DEBUG
	#define __PRINTF	printf
#else
	#define __PRINTF	if (false) printf
#endif

extern int Mouse_x;
extern int Mouse_y;
extern int STEEP_BY_STEEP;

#endif

