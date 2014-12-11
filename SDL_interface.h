#ifndef __SDL_INTERFACE
#define __SDL_INTERFACE


extern SDL_Surface *Screen;
extern SDL_Event event;

// draw pixel on the screen
void putpixel(int x, int y, int color);
void clear_screen(int width, int height);

#endif
