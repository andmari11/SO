#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct ThreadArgs {
    int threadNumber;
    char priority;
};

void *thread_usuario(void *arg)
{

	struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;
    	int threadNumber = threadArgs->threadNumber;
    	char priority = threadArgs->priority;
    
    	printf("Hilo %ld: Número de hilo: %d, Prioridad: %c\n", pthread_self(), threadNumber, priority);
    
    	// Liberar la memoria dinámica reservada para los argumentos
    	free(arg);
    
    	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{

	//int numThreads = argv[0] - '0';
	pthread_t threads[5]; 
    	struct ThreadArgs *args;

    	for (int i = 0; i < 5; i++) {
        	args = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
        	if (args == NULL) {
            		perror("Error en la asignación de memoria");
            	exit(EXIT_FAILURE);
        	}
        	args->threadNumber = i + 1;  // Los números de hilo son impares
       		args->priority = (i % 2 != 0) ? 'P' : 'N';  // Alternar entre P y N
        
        	// Crear el hilo con los argumentos adecuados
        	if (pthread_create(&threads[i], NULL, thread_usuario, args) != 0) {
            	perror("Error al crear el hilo");
            	exit(EXIT_FAILURE);
        	}
    	}

    	// Esperar a que todos los hilos finalicen
    	for (int i = 0; i < 5; i++) {
        	if (pthread_join(threads[i], NULL) != 0) {
            		perror("Error al esperar al hilo");
            		exit(EXIT_FAILURE);
        	}
    	}

    return 0;
}
