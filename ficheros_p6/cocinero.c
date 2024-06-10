#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMITER 3
#define NSALVAJES 5
#define M 10

int servingsDisponibles=0;


int turno_actual=0;
int fila_salvajes=0;
sem_t mutex;
sem_t sem_salvajes, sem_cocinero;

struct tSalvaje{

	int id;
	pthread_t thread;

};

void getServingsFromPot(void)
{
	sem_wait(&mutex);
	int turno= fila_salvajes++;

	while(turno<turno_actual){
        sem_wait(&sem_salvajes);
	}

	while(servingsDisponibles==0){ 

		sem_post(&sem_cocinero);
		sem_wait(&mutex);
	}

	servingsDisponibles--;
	turno_actual++;
	sem_post(&sem_salvajes);
	sem_post(&mutex);
}

void eat(int id)
{
	printf("Savage %lu eating (%d servings left)\n", id, servingsDisponibles);
	sleep(rand() % 5);
}

void* savages(void* arg)
{
	int id = (int) arg;
	printf("+Savage %lu entering\n", id);

	for (int i = 0; i < NUMITER; i++)
	{
		getServingsFromPot();
		eat(id);
	}
	printf("-Savage %lu leaving\n", id);

}


void putServingsInPot(int servings)
{
	printf("--> Putting servings!\n");

	servingsDisponibles=M;
}

void* cook(void* arg)
{
	while(1) {
		sem_wait(&mutex);

		while(servingsDisponibles!=0){
			sem_wait(&sem_cocinero);
		}
		putServingsInPot(M);
		for (int i = 0; i < NSALVAJES; i++) {
			sem_post(&sem_salvajes);
		}
		sem_post(&mutex);

	}
}



int main(int argc, char *argv[])
{
	sem_init(&mutex, 0, 1); // Inicializar el semáforo de mutex
    sem_init(&sem_salvajes, 0, 1); // Inicializar el semáforo de los salvajes
    sem_init(&sem_cocinero, 0, 1);

	pthread_t cocinero;
	pthread_create(&cocinero,NULL, cook, NULL);
	struct tSalvaje* salvajes=malloc(NSALVAJES*sizeof(struct tSalvaje)); 

	for(int i=0;i<NSALVAJES;i++){

		pthread_create(&salvajes[i].thread,NULL, savages, (void*)i);
	}

	for(int i=0;i<NSALVAJES;i++){

		if(pthread_join(salvajes[i].thread, NULL)){

			perror("error al esperar al hilo");
			return -1;
		}
	}

    pthread_cancel(cocinero);
    pthread_join(cocinero, NULL);

	free(salvajes);

    sem_destroy(&mutex);
    sem_destroy(&sem_cocinero);
    sem_destroy(&sem_salvajes);

	return 0;
}
