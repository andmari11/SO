#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 10

pthread_mutex_t mutex;
pthread_cond_t cond_coches_dir1,cond_coches_dir2;

int coches_esperando=0;
int turno_actual=0;

int coches_en_puente=0;
int dir_puente=0;

void enter_bridge(int dir){

    pthread_mutex_lock(&mutex);
    int turno=coches_esperando++;

    while(turno>turno_actual || coches_en_puente>=4 || (dir_puente!=dir && coches_en_puente>0)){
        if(dir)
            pthread_cond_wait(&cond_coches_dir1,&mutex);
        else
            pthread_cond_wait(&cond_coches_dir2,&mutex);

    }
    turno_actual++;
    dir_puente=dir;
    coches_en_puente++;
    printf("Coche %d entra al puente en direccion %d\n", turno_actual, dir);
        if(dir)
            pthread_cond_broadcast(&cond_coches_dir1);
        else
            pthread_cond_broadcast(&cond_coches_dir2);


    pthread_mutex_unlock(&mutex);
}

void cross_bridge(int dir){

    sleep(rand()%3);

}

void exit_bridge(int dir){

    pthread_mutex_lock(&mutex);

    coches_en_puente--;
    printf("Coche con direccion %d sale del puente\n", dir);

    pthread_cond_broadcast(&cond_coches_dir1);
    pthread_cond_broadcast(&cond_coches_dir2);
    pthread_mutex_unlock(&mutex);
}


void* car(void* arg)
{
    unsigned long id=(unsigned long )arg;
    int dir=rand()%2;
    enter_bridge(dir);
    cross_bridge(dir);
    exit_bridge(dir);
    return NULL;
}

int main() {
    if (pthread_mutex_init(&mutex, NULL)) {
        perror("Error al inicializar el mutex\n");
        return -1;
    }

    if (pthread_cond_init(&cond_coches_dir2, NULL)) {
        perror("Error al inicializar la condición\n");
        return -1;
    }
    if (pthread_cond_init(&cond_coches_dir1, NULL)) {
        perror("Error al inicializar la condición\n");
        return -1;
    }
    pthread_t* hilos = malloc(N * sizeof(pthread_t)); 

    for (unsigned long i = 0; i < N; i++) {
        if (pthread_create(&hilos[i], NULL, car, (void*)i)) {
            perror("Error al crear el hilo\n");
            return -1;
        }	
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(hilos[i], NULL)) {
            perror("Error al esperar al hilo\n");
            return -1;
        }	
    }

    free(hilos);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_coches_dir1);
    pthread_cond_destroy(&cond_coches_dir2);

    return 0;
}
