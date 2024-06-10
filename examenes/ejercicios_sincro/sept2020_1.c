#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>



pthread_mutex_t mutex;
pthread_cond_t* conds_Array;
int a=1;
int b=0;
int c=0;
int d=0;
int e=0;




void* A (void* arg){

    pthread_mutex_lock(&mutex);

    while(!a){
        pthread_cond_wait(&conds_Array[0], &mutex);
    }
    printf("Soy el hilo A\n");

    b=1;
    c=1;
    pthread_cond_signal(&conds_Array[1]);
    pthread_cond_signal(&conds_Array[2]);

	pthread_mutex_unlock(&mutex);
}
void* B (void* arg){

    pthread_mutex_lock(&mutex);

    while(!b){
        pthread_cond_wait(&conds_Array[1], &mutex);
    }
    printf("Soy el hilo B\n");

    d++;
    pthread_cond_signal(&conds_Array[3]);

	pthread_mutex_unlock(&mutex);
}
void* C (void* arg){

    pthread_mutex_lock(&mutex);

    while(!c){
        pthread_cond_wait(&conds_Array[2], &mutex);
    }
    printf("Soy el hilo C\n");

    d++;
    e++;
    pthread_cond_signal(&conds_Array[3]);

	pthread_mutex_unlock(&mutex);
}
void* D (void* arg){

    pthread_mutex_lock(&mutex);

    while(d!=2){
        pthread_cond_wait(&conds_Array[3], &mutex);
    }
    printf("Soy el hilo D\n");

    e++;
    pthread_cond_signal(&conds_Array[4]);

	pthread_mutex_unlock(&mutex);
}
void* E (void* arg){

    pthread_mutex_lock(&mutex);

    while(e!=2){
        pthread_cond_wait(&conds_Array[4], &mutex);
    }
    printf("Soy el hilo E\n");


	pthread_mutex_unlock(&mutex);
}


int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex\n");
		return -1;
	}

    conds_Array=malloc(5*sizeof(pthread_cond_t));

    for(int i=0;i<5;i++){
        if(pthread_cond_init(&conds_Array[i], NULL)){
            perror("error cond\n");
            return -1;
        }
    }

	pthread_t* hilos=malloc(5*sizeof(pthread_t)); 


	pthread_create(&hilos[0],NULL, A, NULL);
	pthread_create(&hilos[1],NULL, B, NULL);
	pthread_create(&hilos[2],NULL, C, NULL);
	pthread_create(&hilos[3],NULL, D, NULL);
	pthread_create(&hilos[4],NULL, E, NULL);


	for(int i=0;i<5;i++){

        if(pthread_join(hilos[i],NULL)){

            perror("error al esperar al hilo");
            return -1;
        }	}

	

	pthread_mutex_destroy(&mutex);
    	for(int i=0;i<5;i++){

	    pthread_cond_destroy(&conds_Array[i]);
	}
	free(hilos);
    free(conds_Array);

	return 0;
}
