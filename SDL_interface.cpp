#include "main.h"

#include "Threads.h"
#include "GameOfLife.h"
#include "SDL_interface.h"


SDL_Surface *Screen;
SDL_Event event;


// draw pixel on the screen
void putpixel(int x, int y, int color)
{
    unsigned int *ptr = (unsigned int*)Screen->pixels;
    int lineoffset = y * (Screen->pitch / 4);
    ptr[lineoffset + x] = color;
}

void clear_screen(int width, int height) {
     for (int y=0; y<width; y++)
          for (int x=0; x<height; x++)
              putpixel(x,y,0);

}
