#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define N 10
#define NIMPARES 3


pthread_mutex_t mtx;
pthread_cond_t* impar_array_cond;
pthread_cond_t par_cond;

int turno_impares=0;
int n_actual=1;



void* impar (void* arg){
    unsigned long id=(unsigned long) arg;
    while(1){
        pthread_mutex_lock(&mtx);
        while(n_actual%2==0 || turno_impares!=id){

            pthread_cond_wait(&impar_array_cond[id],&mtx);

            if(n_actual>N-1){
                pthread_mutex_unlock(&mtx);
                return 0;
            }
        }
        printf("Hilo Impar(%d):%d \n",id, n_actual);
        n_actual++;
        turno_impares=(turno_impares+1)%NIMPARES;
        pthread_cond_signal(&par_cond);
        pthread_mutex_unlock(&mtx);
    }

    return 0;
}


void* par (void* arg){
    unsigned long id=(unsigned long) arg;
    while(n_actual<N){
        pthread_mutex_lock(&mtx);
        while(n_actual%2==1){

            pthread_cond_wait(&par_cond,&mtx);
        }
        printf("Hilo Par(%d):%d \n",id, n_actual);
        n_actual++;
        pthread_cond_signal(&impar_array_cond[turno_impares]);
        pthread_mutex_unlock(&mtx);
    }
    for(int i=0;i<NIMPARES;i++)
        pthread_cond_signal(&impar_array_cond[i]);


}


int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mtx, NULL)){
		perror("error mutex\n");
		return -1;
	}
    // || pthread_cond_init(&productor_cond, NULL) 
    if(pthread_cond_init(&par_cond, NULL)){
        perror("error cond\n");
        return -1;
    }
    impar_array_cond=malloc(NIMPARES*sizeof(pthread_cond_t));
    for(int i=0; i<NIMPARES; i++){

        if(pthread_cond_init(&impar_array_cond[i], NULL)){
            perror("error cond\n");
            return -1;
        }
    }
    

	pthread_t* hilos=malloc((NIMPARES+1)*sizeof(pthread_t)); 

    if(pthread_create(&hilos[0],NULL, impar, (void*)0)||pthread_create(&hilos[1],NULL, impar, (void*)1)||
       pthread_create(&hilos[2],NULL, impar, (void*)2)||pthread_create(&hilos[3],NULL, par, (void*)3)){

        perror("error al crear al hilo\n");
        return -1;
    }

	for(int i=0;i<3;i++){

        if(pthread_join(hilos[i],NULL)){

            perror("error al esperar al hilo\n");
            return -1;
        }	
    }


	pthread_mutex_destroy(&mtx);

    pthread_cond_destroy(&par_cond);
    //pthread_cond_destroy(&impar_array_cond[]);

	free(hilos);
    free(impar_array_cond);
	return 0;
}
