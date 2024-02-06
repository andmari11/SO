#include "header.h"



int getServingsFromPot(void)
{

	informacion->servings--;
}

void eat(void)
{
	unsigned long id = (unsigned long)getpid();
	printf("Savage %lu eating serving %d\n", id, informacion->servings);
	sleep(rand() % 5);
}

void savages(void)
{
	for (int i = 0; i < NUMITER; i++)
	{
		// lock(mutex)
		// sem_trywait(sem_mutex);
		sem_wait(sem_mutex);

		while (informacion->servings <= 0)
		{
			informacion->salvajes_waiting++;
			printf("Esperando a comer\n");

			while (informacion->cocineros_waiting > 0)
			{
				informacion->cocineros_waiting--;
				sem_post(semCocinero);
			}

			/*equivalente a cond_signal*/

			// unlock(mutex)
			sem_post(sem_mutex);
			
			sem_wait(semSalvaje);
			// lock(mutex)
			sem_wait(sem_mutex);
		}
		getServingsFromPot();
		eat();

		//sem_post(semSalvaje);
		sem_post(sem_mutex);
	}
}

int main(int argc, char *argv[])
{
	//semaforos con nombre
	sem_mutex = sem_open("/mutex", O_RDWR);
	semCocinero = sem_open("/cocinerosMemoria", O_RDWR);
	semSalvaje = sem_open("/salvajesMemoria", O_RDWR);

	if (sem_mutex == SEM_FAILED || semCocinero == SEM_FAILED || semSalvaje == SEM_FAILED)
	{

		perror("sem");
		exit(EXIT_FAILURE);
	}

	//abre archivo  compartido
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

	//ejecuta  programa
	savages();

	//cerrar semaforos
	sem_close(sem_mutex);
	sem_close(semCocinero);
	sem_close(semSalvaje);

	return 0;
}
