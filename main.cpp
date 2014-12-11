// ----------------------------------------------------------------------------------
// Game of Life - SDL - Multithreads
// Date: 11.12.2014
// Version: none
// Author: Maciej Dziemianczuk
// ----------------------------------------------------------------------------------
#include "main.h"
#include "Threads.h"
#include "GameOfLife.h"
#include "SDL_interface.h"

//#define __DEBUG

// -----------------------------------------------------------------------------------
void Init()
{
	__PRINTF("Init()\n");


	// Number of threads
	NUM_THREADS = 4;

	// Screen resolution
	World.Width = 1000;
	World.Height = 600;
}

// -----------------------------------------------------------------------------------
void Help()
{
	printf("Game of Life\n\n");
	printf("Keyboard\\Mouse:\n\n");
	printf("C - clear screen\n");
	printf("G - draw glider\n");
	printf("LeftMouse - draw random cells\n");
	printf("MiddleMouse - draw cross of cells\n");
	printf("RightMouse - draw square of cells\n");
}



// ----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	Init();

    // Initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    // Register SDL_Quit to be called at exit;
    atexit(SDL_Quit);

    // Attempt to create a WIDTH x HEIGHT window with 32bit pixels depth
    Screen = SDL_SetVideoMode(World.Width, World.Height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Game of Life", NULL);

    // init memory/arrays and so on
    InitWorld(World);

    // init parallel stuff: SDL_threads, mutex, etc
    InitParallel();

	short isExit = 0;
    while (!isExit)
    {
        // Lock surface if needed
        if (SDL_LockSurface(Screen) < 0) return 1;


        // evolution - main
        if (STEEP_BY_STEEP == 0)
        {
            // Calculate the Second from the First
            // Show the Second
            // Copy from the Second to the First
            EvolutionParallel();

        }

        // Unlock if needed
        if (SDL_MUSTLOCK(Screen))
           SDL_UnlockSurface(Screen);


        // Tell SDL to update the whole screen
        SDL_UpdateRect(Screen, 0, 0, World.Width, World.Height);


        // Poll for events, and handle the ones we care about
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_F1) Help();

					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						isExit = 1;
						break;
					}


					if (event.key.keysym.sym == SDLK_SPACE)
						Action = _RANDOM;

					if (event.key.keysym.sym == SDLK_s)
					{
						STEEP_BY_STEEP = (STEEP_BY_STEEP + 1) % 2;
					}
					if (event.key.keysym.sym == SDLK_g)
					{
						SDL_GetMouseState(&Mouse_x, &Mouse_y);
						Action = _GLIDER;
					}


					if (event.key.keysym.sym == SDLK_c)
						Action = _CLEAR;

					if (event.key.keysym.sym == SDLK_n)
						EvolutionParallel();

					if (event.key.keysym.sym == SDLK_r)
						Action = _RANDOMALL;

					break;


				case SDL_MOUSEBUTTONDOWN:

					Mouse_x = event.button.x;
					Mouse_y = event.button.y;

					if (event.button.button == SDL_BUTTON_LEFT)
					{
						Action = _SHAPE1;
					}
					if (event.button.button == SDL_BUTTON_RIGHT)
					{
						Action = _SHAPE2;
					}
					if (event.button.button == SDL_BUTTON_MIDDLE)
					{
						Action = _SHAPE3;
					}
					break;

				case SDL_QUIT:
    				return 0;
					break;
            }
        }

    }


    CleanParallel();
    CleanWorld(World);

    return 0;
}

// -----------------------------------------------------------------------------------
void Exit()
{
	__PRINTF("Exit()\n");
	CleanParallel();
	CleanWorld(World);
}
