#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define NJUGADORES 4
#define TOPEPUNTOS 3
#define MAXREBOTE 70

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t jugadores_cond = PTHREAD_COND_INITIALIZER;

enum estadoBalon_t { POSESION, ENAIRE };
enum estadoBalon_t balon=ENAIRE; // Modela el estado actual del balón
int dificultadRebote = 0; // var. Global para indicar la dificultad del rebote actual
bool finished = false;

struct jugador {
    int dorsal;     // dorsal (único) del jugador
    int eficTiro;   // entero entre 1 y 100 que determina su efectividad en el tiro
    int eficRebote; // entero entre 1 y 100 que determina su efectividad en el rebote
    int puntos;     // puntos conseguidos durante el entrenamiento
};

struct jugador jugadores[NJUGADORES] = {
    {0, 30, MAXREBOTE, 0}, // jugador con dorsal 0, eficTiro 30, eficRebote MAXREBOTE
    {1, 80, 20, 0},
    {2, 40, MAXREBOTE, 0},
    {3, 50, 50, 0}
};

void rebotear(struct jugador *j) {
    pthread_mutex_lock(&mutex);

    // Esperar a conseguir rebote
    while (balon == POSESION || dificultadRebote > j->eficRebote ) {
        pthread_cond_wait(&jugadores_cond, &mutex);
    }
    if (finished) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    printf("Jugador %d ha conseguido el rebote\n", j->dorsal);
    balon = POSESION;
    pthread_mutex_unlock(&mutex);
}

void correr() {
    usleep(100);
}

void tirar(struct jugador *j) {
    pthread_mutex_lock(&mutex);
    int difTiro = rand() % 100;
    // Si difTiro es menor que nuestra eficacia en tiro: canasta
    if (j->eficTiro >= difTiro) {
        j->puntos++;
        printf("Jugador %d ha marcado! Puntos: %d\n", j->dorsal, j->puntos);
        if (j->puntos >= TOPEPUNTOS)
        {
            printf("Jugador %d ha ganado el juego!\n", j->dorsal);
            finished = true;
            dificultadRebote = 0;
            balon = ENAIRE;
        }
    }
    if (!finished)
    {
        balon = ENAIRE;
        dificultadRebote = rand() % MAXREBOTE;
        printf("Balón en el aire, dificultad de rebote: %d\n", dificultadRebote);
    }


    pthread_cond_broadcast(&jugadores_cond);
    pthread_mutex_unlock(&mutex);
}

void *jugadorInit(void *arg) {
    struct jugador *j = (struct jugador *)arg;
    printf("Jugador %d-> efictiro: %d, eficrebote: %d, puntos: %d\n", j->dorsal, j->eficTiro, j->eficRebote, j->puntos);

    while (!finished) {
        rebotear(j);
        correr();
        tirar(j);
        correr();
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t *hilos = malloc(NJUGADORES * sizeof(pthread_t));

    for (int i = 0; i < NJUGADORES; i++) {
        if (pthread_create(&hilos[i], NULL, jugadorInit, (void *)&jugadores[i])) {
            return -1;
        }
    }

    for (int i = 0; i < NJUGADORES; i++) {
        if (pthread_join(hilos[i], NULL)) {
            perror("error al esperar al hilo\n");
            return -1;
        }
    }

    free(hilos);
    return 0;
}
