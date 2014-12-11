#ifndef __GAME_OF_LIFE
#define __GAME_OF_LIFE

enum _ACTIONS
{
    _NONE, _SHAPE1, _SHAPE2, _SHAPE3, _GLIDER, _RANDOM,
    _CLEAR, _RANDOMALL,
};

extern enum _ACTIONS Action;

typedef struct SWorld
{
    unsigned short Width;
    unsigned short Height;

    // Array for calculation
    char *ArrayFirst;
    char *ArraySecond;

    // Array for colors of cells
    short *Colors;


} SWorld;

extern SWorld World;

int ComputeParallel(void *_ptrThread);
void EvolutionParallel(void);
int Neighbor(int x, int y, char* mem);
int Evolution(int height_start, int height_end);

void random_world(char* mem, int number);

void ApplyActions(void);

void new_cells(char* mem, int number);
void pick_cells(char* mem, int x, int y);
void shape1(char* mem, int x, int y);
void shape2(char* mem, int x, int y);
void glider(char* mem, int x, int y);
void clear_world(void);
void CopyWorld(void);
void InitWorld(SWorld &world);
void CleanWorld(SWorld &world);



#endif
