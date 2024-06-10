#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define N 10

pthread_mutex_t mutex;
pthread_cond_t productor_cond;
pthread_cond_t consumidor_cond;

bool consumir_bool=false;
int n_actual=0;



void* productor (void* arg){
    unsigned long par=(unsigned long)arg;
    printf("Productor %lu iniciado\n", par);
    while(n_actual<N-par){
        pthread_mutex_lock(&mutex);

        while(consumir_bool || n_actual%2!=par){

            pthread_cond_wait(&productor_cond,&mutex);
        }
        n_actual++;
        consumir_bool=true;

        pthread_cond_broadcast(&consumidor_cond);
        pthread_mutex_unlock(&mutex);
    }
}


void* consumer (void* arg){

    while(n_actual<N){
        pthread_mutex_lock(&mutex);

        while(!consumir_bool){

            pthread_cond_wait(&consumidor_cond,&mutex);
        }
        printf("%d\n", n_actual);
        consumir_bool=false;
        pthread_cond_broadcast(&productor_cond);
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex\n");
		return -1;
	}
    // || pthread_cond_init(&productor_cond, NULL) 
    if(pthread_cond_init(&consumidor_cond, NULL)|| pthread_cond_init(&productor_cond, NULL)){
        perror("error cond\n");
        return -1;
    }
    

	pthread_t* hilos=malloc(3*sizeof(pthread_t)); 

    if(pthread_create(&hilos[0],NULL, consumer, NULL) ||	pthread_create(&hilos[1],NULL, productor, (void*)0)||pthread_create(&hilos[2],NULL, productor, (void*)1)){

        perror("error al crear al hilo\n");
        return -1;
    }

	for(int i=0;i<3;i++){

        if(pthread_join(hilos[i],NULL)){

            perror("error al esperar al hilo\n");
            return -1;
        }	
    }


	pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&consumidor_cond);
    pthread_cond_destroy(&productor_cond);
    //pthread_cond_destroy(&productor2_cond);

	free(hilos);

	return 0;
}
