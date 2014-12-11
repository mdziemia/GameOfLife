#include "main.h"


#include "Threads.h"
#include "GameOfLife.h"
#include "SDL_interface.h"

#define TAB(x,y,mem) *(mem+World.Width*(y)+(x))

const int COLOR = 0xff0000;     // color of cells
const int COLOR_NEW = 0x00ff00; // color of born cells
const int COLOR_BG = 0x000000;
const int COLOR_DEAD = 0x000000;

enum _ACTIONS Action;

SWorld World;

int STEEP_BY_STEEP = 0;
int Mouse_x, Mouse_y;

// ----------------------------------------------------------------------------------
// Drawing pixel at (x,y)
// before and after specify the state of a cell
void DrawPixel(int x, int y, bool before, bool after)
{
    if (before)
    {
        if (after)
        {
            if (TAB(x,y,World.Colors) > 0)
                TAB(x,y,World.Colors) = TAB(x,y,World.Colors) - 1;
            else
                TAB(x,y,World.Colors) = 0;

            putpixel(x,y,0xff00ff + TAB(x,y,World.Colors));
        }
        else
        {
            TAB(x,y,World.Colors) = -255;
            putpixel(x,y,- TAB(x,y,World.Colors)/2);
        }
    }
    else
    {
        if (after)
        {
            TAB(x,y,World.Colors) = 255;
            putpixel(x,y,0xff00ff + TAB(x,y,World.Colors));
        }
        else
        {
            if (TAB(x,y,World.Colors) < 0)
                TAB(x,y,World.Colors) = TAB(x,y,World.Colors) + 1;
            else
                TAB(x,y,World.Colors) = 0;

            putpixel(x,y,- TAB(x,y,World.Colors)/2);
        }
    }
}

// ----------------------------------------------------------------------------------
int Evolution(int height_start, int height_end)
{
	int x,y;
	unsigned char n = 0;

	for (y=height_start; y<height_end; ++y)
	{
		for (x=0; x<World.Width; ++x)
		{
			n = Neighbor(x,y,World.ArrayFirst);
			if (TAB(x,y,World.ArrayFirst))
			{
				// RULES FOR ALIAVE CELLS
				if (n == 2 || n == 3)
				{
                    // cell still lives
                    TAB(x,y,World.ArraySecond) = 1;
                    DrawPixel(x,y,1,1);
				}
				else
				{
                    //cell must die
					TAB(x,y,World.ArraySecond) = 0;
					DrawPixel(x,y,1,0);
                }
			}
			else
			{
				// RULES FOR DEAD CELLS
				if (n == 3)
				{
					// cell that come back to the game
					TAB(x,y,World.ArraySecond) = 1;
					DrawPixel(x,y,0,1);
				}
				else
				{
                    TAB(x,y,World.ArraySecond) = 0;
                    DrawPixel(x,y,0,0);
				}
			}
		}
	}
	return 0;
}



// ----------------------------------------------------------------------------------
// This is the main method which runs at every THREAD
int ComputeParallel(void *_ptrThread)
{
    if (!_ptrThread) return 0;
    SThread *ptrThread = (SThread *)_ptrThread;


    // calculate range of rows to deal with
    int range_of_height = (int)(World.Height / NUM_THREADS);
    int height_start = ptrThread->id*range_of_height;
    int height_end = (ptrThread->id+1)*range_of_height;
    if (ptrThread->id == NUM_THREADS - 1)
        height_end = World.Height;


    // main loop
    while (ptrThread->status != THREAD_STATUS_QUIT)
    {

        SDL_LockMutex(ptrThread->hMutex);
        if (ptrThread->status == THREAD_STATUS_WAIT)
        {
            SDL_UnlockMutex(ptrThread->hMutex);
            continue;
        }
        else
        {
            ptrThread->status = THREAD_STATUS_WAIT;
            SDL_UnlockMutex(ptrThread->hMutex);
        }


        // Calculation at specified range
        Evolution(height_start, height_end);


        // Critical Section
        SDL_LockMutex(lockSynchMutex);
            cSynchronize ++;
        SDL_UnlockMutex(lockSynchMutex);

    }
    return 0;
}

// ----------------------------------------------------------------------------------
void EvolutionParallel(void)
{

    // Critical section
    SDL_LockMutex(lockSynchMutex);
    if (cSynchronize != NUM_THREADS)
    {
        SDL_UnlockMutex(lockSynchMutex);
        return;
    }
    cSynchronize = 0;
    SDL_UnlockMutex(lockSynchMutex);

    // Apply actions from Keyboard/Mouse
    ApplyActions();

    // Copy from Second to First
    CopyWorld();

    for (int i=0; i<NUM_THREADS; ++i)
    {
        // Critical section
        SDL_LockMutex(arrThreads[i].hMutex);
            arrThreads[i].status = THREAD_STATUS_DEFAULT;
        SDL_UnlockMutex(arrThreads[i].hMutex);
    }


    //SDL_UnlockMutex(hMutex);
}

// ----------------------------------------------------------------------------------
void ApplyActions(void)
{
    switch (Action)
    {
        case _SHAPE1:
            pick_cells(World.ArraySecond, Mouse_x, Mouse_y);
            break;

        case _SHAPE2:
            shape1(World.ArraySecond, Mouse_x, Mouse_y);
            break;

        case _SHAPE3:
            shape2(World.ArraySecond, Mouse_x, Mouse_y);
            break;

		case _GLIDER:
			glider(World.ArraySecond, Mouse_x, Mouse_y);
			break;

        case _CLEAR:
            clear_world();
            break;

        case _RANDOMALL:
            random_world(World.ArraySecond, 10000);
            break;

        case _RANDOM:
            new_cells(World.ArraySecond, 200);
            break;

		default:
			break;

    }
    //pick_cells(World.ArrayFirst, event.button.x, event.button.y);
	//pick_cells(World.ArraySecond, event.button.x, event.button.y);
    Action = _NONE;
}

// ----------------------------------------------------------------------------------
// gives the number of neighbour alive cells
int Neighbor(int x, int y, char* mem) {
	int count = 0;

	//   7 4 8
	//   3 X 1
	//   6 2 5

	if (TAB((x+1)%World.Width,y,mem)) count ++; // 1
	if (TAB(x,(y+1)%World.Height,mem)) count ++; // 2

	if (x == 0) {
       if (TAB(World.Width-1,y,mem)) count ++; // 3
       if (TAB(World.Width-1,(y+1)%World.Height,mem)) count ++; // 6
       if (y == 0) { // 7
           if (TAB(World.Width-1,World.Height-1,mem)) count ++;
       } else {
           if (TAB(World.Width-1,y-1,mem)) count ++;
       }
    } else {
       if (TAB(x-1,y,mem)) count ++; // 3
       if (TAB(x-1,(y+1)%World.Height,mem)) count ++; // 6
       if (y == 0) { // 7
           if (TAB(x-1,World.Height-1,mem)) count ++;
       } else {
           if (TAB(x-1,y-1,mem)) count ++;
       }
    }

	if (y == 0) {
          if (TAB(x,World.Height-1,mem)) count ++; // 4
          if (TAB((x+1)%World.Width,World.Height-1,mem)) count ++; // 8
    } else {
          if (TAB(x,y-1,mem)) count ++; // 4
          if (TAB((x+1)%World.Width,y-1,mem)) count ++; // 8
    }

    if (TAB((x+1)%World.Width,(y+1)%World.Height,mem)) count ++; // 5

	return count;
}



// ----------------------------------------------------------------------------------
// generate random world
void random_world(char* mem, int number)
{
     int x,y;

     for (int i = 0; i < number; i++) {
         x = rand() % World.Width;
         y = rand() % World.Height;
         // create new alive cell
         if (!TAB(x,y,mem)) {
            TAB(x,y,mem) = 1;
            putpixel(x,y,COLOR_NEW);
         }
     }
}

// ----------------------------------------------------------------------------------
// create new random cells to complicate our world :)
void new_cells(char* mem, int number)
{
     int xc,yc,x,y;
     xc = rand() % (World.Width - (int)sqrt(number));
     yc = rand() % (World.Height - (int)sqrt(number));
     for (int i=0; i<number; i++) {
         x = rand() % (int)sqrt(number);
         y = rand() % (int)sqrt(number);
         TAB(xc+x,yc+y,mem) = 1;
         putpixel(xc+x,yc+y,COLOR);
     }
}

// ----------------------------------------------------------------------------------
void pick_cells(char* mem, int x, int y)
{
    int number = 2000;
    int xd, yd;

    for (int i=0; i<number; i++) {
         xd = rand() % (int)sqrt(number);
         yd = rand() % (int)sqrt(number);
         TAB(xd+x,yd+y,mem) = 1;
         //putpixel(xd+x,yd+y,COLOR_NEW);
     }

    TAB(x,y,mem) = 1;
    //putpixel(x,y,COLOR);
}


// ----------------------------------------------------------------------------------
void shape1(char* mem, int x, int y)
{
    int number = rand()%200;
	int i,j;

    for (i=0; i<=2*number; i++) {
         for (j=0;j<=2*number; ++j)
         {

            TAB((World.Width-number+x+i)%World.Width,(World.Height-number+y+j)%World.Height,mem) = 0;
            //putpixel((World.Width-number+x+i)%World.Width,(World.Height-number+y+j)%World.Height,127);
         }
     }

     for (i=0;i<=2*number; ++i)
     {
         TAB((World.Width-number+x+i)%World.Width,(World.Height-number+y)%World.Height,mem) = 1;
         TAB((World.Width-number+x+i)%World.Width,(World.Height-number+y+2*number)%World.Height,mem) = 1;

         TAB((World.Width-number+x)%World.Width,(World.Height-number+y+i)%World.Height,mem) = 1;
         TAB((World.Width-number+x+2*number)%World.Width,(World.Height-number+y+i)%World.Height,mem) = 1;
     }
}

// ----------------------------------------------------------------------------------
void shape2(char* mem, int x, int y)
{
    int number = rand()%200;
    int  i;


    for (i=0; i<=2*number; i++) {

        TAB((x)%World.Width,(World.Height-number+y+i)%World.Height,mem) = 1;
        //putpixel((x)%World.Width,(World.Height-number+y+i)%World.Height,127);

        TAB((World.Width-number+x+i)%World.Width,(y)%World.Height,mem) = 1;
        //putpixel((World.Width-number+x+i)%World.Width,(y)%World.Height,127);


    }
}

// ----------------------------------------------------------------------------------
// .X
//   X
// XXX
void glider(char* mem, int x, int y)
{
	TAB((x+1)%World.Width,(y)%World.Height,mem) = 1;
	TAB((x+2)%World.Width,(y+1)%World.Height,mem) = 1;
	TAB((x)%World.Width,(y+2)%World.Height,mem) = 1;
	TAB((x+1)%World.Width,(y+2)%World.Height,mem) = 1;
	TAB((x+2)%World.Width,(y+2)%World.Height,mem) = 1;
}
// ----------------------------------------------------------------------------------
void clear_world(void)
{
    memset(World.ArrayFirst, 0, World.Width*World.Height*sizeof(char));
    memset(World.ArraySecond, 0, World.Width*World.Height*sizeof(char));

    //clear_screen(World.Width, World.Height);
}


// ----------------------------------------------------------------------------------
void CopyWorld(void)
{
    memcpy(World.ArrayFirst,World.ArraySecond,World.Width*World.Height*sizeof(char));

}

// ----------------------------------------------------------------------------------
void InitWorld(SWorld &world)
{
    world.ArrayFirst = (char *)malloc(sizeof(char) * world.Width * world.Height);
    world.ArraySecond = (char *)malloc(sizeof(char) * world.Width * world.Height);
    world.Colors = (short *)malloc(sizeof(short) * world.Width * world.Height);
}

// ----------------------------------------------------------------------------------
void CleanWorld(SWorld &world)
{
    free (world.ArrayFirst);
    free (world.ArraySecond);
    free (world.Colors);
}
