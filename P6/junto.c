#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUMITER 3
#define NSALVAJES 5
#define M 10

int servingsDisponibles=0;


int turno_actual=0;
int fila_salvajes=0;

pthread_mutex_t mutex;
pthread_cond_t condSalvajes, condCocinero;

struct tSalvaje{

	int id;
	pthread_t thread;

};

void getServingsFromPot(void)
{
	pthread_mutex_lock(&mutex);
	int turno= fila_salvajes++;

	while(turno<turno_actual){

		pthread_cond_wait(&condSalvajes, &mutex);
	}

	while(servingsDisponibles==0){

		pthread_cond_signal(&condCocinero);
		pthread_cond_wait(&condSalvajes, &mutex);
	}

	servingsDisponibles--;
	turno_actual++;
	pthread_cond_signal(&condSalvajes);
	pthread_mutex_unlock(&mutex);
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
        pthread_mutex_lock(&mutex);

		while(servingsDisponibles!=0){
			pthread_cond_wait(&condCocinero, &mutex);
		}
		putServingsInPot(M);
		pthread_cond_signal(&condSalvajes);
        pthread_mutex_unlock(&mutex);

	}
}



int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex");
		return -1;
	}
	if(pthread_cond_init(&condSalvajes, NULL) || pthread_cond_init(&condCocinero, NULL)){
		perror("error cond");
		return -1;
	}
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

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condSalvajes);
	pthread_cond_destroy(&condCocinero);

	return 0;
}
