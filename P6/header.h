#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

#define M 2
#define NUMITER 1100
#define SHARED_MEMORY "./memoria"

struct tInfo{

    int cocineros_waiting;
    int salvajes_waiting;
    int servings;
};

int shared_memory_fd, finish=0;
sem_t *sem_mutex,*semCocinero, *semSalvaje;
struct tInfo *informacion;