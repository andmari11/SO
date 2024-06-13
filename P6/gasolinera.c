#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>



#define NCLIENTES 10
#define NPUMPS 3

int pumps_en_uso=0;
int turno_actual=0;
int cola=0;
pthread_mutex_t mutex;
pthread_cond_t cliente_cond;

void PumpFuel(int pump, int id){
    
    printf("Cliente %d repostando\n", id);
    sleep(1);

}

int getUnusedPump(int id)
{

    pthread_mutex_lock(&mutex);
    int turno=cola++;
    printf("Cliente %d esperando turno %d\n", id, turno);

    while (pumps_en_uso == NPUMPS || turno_actual!=turno)
    {

        pthread_cond_wait(&cliente_cond, &mutex);
    }
    int ret=pumps_en_uso++;
    turno_actual++;
    pthread_mutex_unlock(&mutex);
    return ret;
}

void releasePump(int id){
    pthread_mutex_lock(&mutex);
    printf("Cliente %d se pira\n", id);
    pumps_en_uso--;
    pthread_cond_broadcast(&cliente_cond);
    pthread_mutex_unlock(&mutex);

}
void *cliente(void* arg)
{
    unsigned long id=(unsigned long) arg;
    int pump= getUnusedPump(id);
    /* Uso del surtidor */
    PumpFuel(pump, id);
    /* Deja de usar el surtidor */
    releasePump(id);
}


int main(int argc, char *argv[]) {
    if (pthread_mutex_init(&mutex, NULL)) {
        perror("Error inicializando el mutex\n");
        return -1;
    }

    if (pthread_cond_init(&cliente_cond, NULL)) {
        perror("Error inicializando la condición\n");
        return -1;
    }

    pthread_t hilos[NCLIENTES];

    for (unsigned long i = 0; i < NCLIENTES; i++) {
        if (pthread_create(&hilos[i], NULL, cliente, (void*)i)) {
            perror("Error al crear el hilo\n");
            return -1;
        }
    }

    for (int i = 0; i < NCLIENTES; i++) {
        if (pthread_join(hilos[i], NULL)) {
            perror("Error al esperar el hilo\n");
            return -1;
        }
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cliente_cond);

    return 0;
}