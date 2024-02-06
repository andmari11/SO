#include "header.h"




void putServingsInPot(int servings)
{
	informacion->servings=servings;
}

// bucle infinito
void cook(void)
{
	while (finish!=1)
	{
		//lock
		sem_wait(sem_mutex);
		while (informacion->servings > 0)
		{
			informacion->cocineros_waiting++;
			printf("cocinero %d, waiting\n", informacion->cocineros_waiting);

			/*equivalente a cond_signal*/

			//unlock
			sem_post(sem_mutex);
			//wait cocineros
			sem_wait(semCocinero);
			//lock
			sem_wait(sem_mutex);


			if(finish==1)
				return;
		}

		putServingsInPot(M);
		printf("%d raciones servidas!\n", M);


		while(informacion->salvajes_waiting!=0){

			sem_post(semSalvaje);
			informacion->salvajes_waiting--;
		}


		//unlock
		sem_post(sem_mutex);
	}
}

void handler(int signo)
{
	finish = 1;
	printf("saliendo\n");
	sem_post(semCocinero);
}

int main(int argc, char *argv[])
{

	//  manejador de señales
    struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;

	//semaforo con nombre
	sem_mutex=sem_open("/mutex", O_CREAT , 0644, 0);
	semCocinero=sem_open("/cocinerosMemoria", O_CREAT , 0644, 0);
	semSalvaje=sem_open("/salvajesMemoria", O_CREAT , 0644, 0);
	
	if(sem_mutex==SEM_FAILED || semCocinero==SEM_FAILED || semSalvaje==SEM_FAILED){

		perror("sem");
        exit(EXIT_FAILURE);
	}

	//archivo memoria compartida
    int shm_fd = shm_open("/memory", O_CREAT | O_RDWR, 0644);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

	ftruncate(shm_fd, sizeof(struct tInfo));
    informacion = mmap(NULL, sizeof(struct tInfo), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (informacion == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

	//reseteamos las variables
	informacion->cocineros_waiting=0;
	informacion->salvajes_waiting=0;
	informacion->servings=0;

	//funcion
	cook();

	//cerrar semaforos
    sem_close(sem_mutex);
    sem_close(semCocinero);
	sem_close(semSalvaje);
    sem_unlink("/mutex");
    sem_unlink("/cocinerosMemoria");
    sem_unlink("/salvajesMemoria");
    shm_unlink("/memory");
	return 0;
}
