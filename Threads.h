#ifndef __THREADS
#define __THREADS

// Parallels
extern unsigned short NUM_THREADS;

enum ThreadStatus {
    THREAD_STATUS_DEFAULT = 0,
    THREAD_STATUS_WAIT = 1,
    THREAD_STATUS_QUIT = 9,
};

typedef struct SThread
{
    SDL_Thread *handle;
    unsigned short id;
    enum ThreadStatus status;
    SDL_mutex *hMutex;
    SDL_cond *cond;

} SThread;

extern unsigned short cSynchronize;
//extern SDL_mutex *hMutex;
extern SDL_mutex *lockSynchMutex;
//extern SDL_mutex *hMutexScreen;

void InitParallel(void);
void CleanParallel(void);


extern SThread *arrThreads;


#endif
