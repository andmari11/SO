#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


// Definición de la estructura barrier_t
typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int n;
} barrier_t;

// Inicialización de la barrera
void barrier_init(barrier_t* b, int nthreads) {
	if(pthread_mutex_init(&b->mtx, NULL)){
		perror("error mutex\n");
		exit -1;
	}
    if(pthread_cond_init(&b->cond, NULL)){
        perror("error cond\n");
        exit -1;
    }
    b->n=0;
}

// Función barrier_wait
void barrier_wait(barrier_t* b) {

    sleep(rand() % 5);


    pthread_mutex_lock(&b->mtx);
    printf("Hilo %lu ha llegado a la barrera\n", pthread_self());
    b->n++;
    while(b->n<5){
        pthread_cond_wait(&b->cond,&b->mtx);
    }
    pthread_cond_broadcast(&b->cond);
    printf("Hilo %lu ha pasado la barrera\n", pthread_self());
    pthread_mutex_unlock(&b->mtx);
}

// Función que será ejecutada por los hilos
void* thread_func(void* arg) {
    barrier_t* barrier = (barrier_t*) arg;

    barrier_wait(barrier);

    return NULL;
}

int main(int argc, char *argv[]) {
    int nthreads = 5;
    pthread_t threads[nthreads];
    barrier_t barrier;

    // Inicialización de la barrera
    barrier_init(&barrier, nthreads);

    // Creación de los hilos
    for (int i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, (void*)&barrier)) {
            perror("Error al crear el hilo\n");
            return -1;
        }
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], NULL)) {
            perror("Error al esperar el hilo\n");
            return -1;
        }
    }

    // Destruir la barrera (si es necesario)

    return 0;
}
