#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define N 5

bool palillos[N] = {true, true, true, true, true};

pthread_mutex_t mutex;
pthread_cond_t cond_filosofos[N];

void cogerPalillosMonitor(int id) {
    pthread_mutex_lock(&mutex);

    while (!palillos[(id + 1) % N] || !palillos[id]) {
        pthread_cond_wait(&cond_filosofos[id], &mutex);
    }
    palillos[(id + 1) % N] = false;
    palillos[id] = false;

    printf("Filósofo %d ha cogido los palillos %d y %d\n", id, id, (id + 1) % N);

    pthread_mutex_unlock(&mutex);
}

void dejarPalillosMonitor(int id) {
    pthread_mutex_lock(&mutex);

    palillos[(id + 1) % N] = true;
    palillos[id] = true;

    printf("Filósofo %d ha dejado los palillos %d y %d\n", id, id, (id + 1) % N);

    pthread_cond_signal(&cond_filosofos[(id + 1) % N]);
    pthread_cond_signal(&cond_filosofos[id]);

    pthread_mutex_unlock(&mutex);
}

void* filosofo(void *arg) {
    unsigned long id = (unsigned long) arg;
    while (1) {
        sleep(rand() % 5);
        /* Solicitamos al monitor la necesidad de coger los palillos */
        cogerPalillosMonitor(id);
        
        printf("Filósofo %lu está comiendo\n", id);
        sleep(rand() % 5);

        /* Solicitud al monitor que queremos dejar los palillos */
        dejarPalillosMonitor(id);
    }
}

int main() {
    if (pthread_mutex_init(&mutex, NULL)) {
        perror("Error al inicializar el mutex\n");
        return -1;
    }

    for (int i = 0; i < N; i++) {
        if (pthread_cond_init(&cond_filosofos[i], NULL)) {
            perror("Error al inicializar la condición\n");
            return -1;
        }
    }

    pthread_t* hilos = malloc(N * sizeof(pthread_t)); 

    for (unsigned long i = 0; i < N; i++) {
        if (pthread_create(&hilos[i], NULL, filosofo, (void*)i)) {
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

    return 0;
}
