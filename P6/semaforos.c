#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

#define N 10

sem_t sem_producir_par;
sem_t sem_producir_impar;
sem_t sem_consumir;

int n_actual = 0;

void* productor(void* arg) {
    unsigned long par = (unsigned long)arg;
    printf("Productor %lu iniciado\n", par);
    sem_t* sem_actual = (par == 0) ? &sem_producir_par : &sem_producir_impar;

    while (n_actual < N - par) {
        sem_wait(sem_actual);        
        n_actual++;        
        sem_post(&sem_consumir);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (n_actual < N) {
        sem_wait(&sem_consumir);
        
        printf("Consumiendo: %d\n", n_actual);
        
        if (n_actual % 2 == 0) {
            sem_post(&sem_producir_impar);
        } else {
            sem_post(&sem_producir_par);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    sem_init(&sem_producir_par, 0, 1);
    sem_init(&sem_producir_impar, 0, 0);
    sem_init(&sem_consumir, 0, 0);

    pthread_t hilos[3];

    if (pthread_create(&hilos[0], NULL, consumer, NULL) ||
        pthread_create(&hilos[1], NULL, productor, (void*)0) ||
        pthread_create(&hilos[2], NULL, productor, (void*)1)) {
        perror("error al crear el hilo\n");
        return -1;
    }

    for (int i = 0; i < 3; i++) {
        if (pthread_join(hilos[i], NULL)) {
            perror("error al esperar al hilo\n");
            return -1;
        }
    }

    sem_destroy(&sem_producir_par);
    sem_destroy(&sem_producir_impar);
    sem_destroy(&sem_consumir);

    return 0;
}
