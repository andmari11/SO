#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct Thread{
	int number;
	char priority;
};
void *thread_usuario(void *arg)
{
struct Thread*args = (struct Thread*)arg;
pthread_t id = pthread_self();
printf("IDHilo:%lu-Numero de hilo:%d-Prioridad:%c\n",id,args->number,args->priority);
pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
int numThreads = 5;
pthread_t threads[numThreads];
struct Thread*args;

for(int i = 0; i < numThreads;i++){
	args = (struct Thread *)malloc(sizeof(struct Thread));
	args->number = i+1;
	if(i%2==1){
	args->priority = 'P';
	}
	else{
	args->priority = 'N';
	}
	
	if(pthread_create(&threads[i],NULL,thread_usuario,args) != 0){
		perror("Error al crear el hilo");
		exit(1);
	}
}
for(int i = 0; i < 5;i++){
	if(pthread_join(threads[i],NULL) != 0){
		perror("Error espera del hilo");
		exit(1);
	}
}
	return 0;
}
