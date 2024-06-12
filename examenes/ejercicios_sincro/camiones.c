#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define NP 10

pthread_mutex_t mutex;
pthread_cond_t truck_cond;
pthread_cond_t worker_cond;

int id_ocupado=-1;
int capacidades[NP];

int turno_actual=0;
int camiones_esperando=0;



void* worker (void* arg){
    unsigned long id=(unsigned long)arg;
    printf("Trabajador %lu iniciado\n", id);


    while(1){
        pthread_mutex_lock(&mutex);

        while(id_ocupado<0 || capacidades[id_ocupado]==NP){

            pthread_cond_wait(&worker_cond,&mutex);
        }
        capacidades[id_ocupado]+=2;        
        printf("Trabajador %lu cargando(%d/%d)\n", id,capacidades[id_ocupado],NP);


        pthread_cond_signal(&worker_cond);    
        if(capacidades[id_ocupado]==NP){
            pthread_cond_broadcast(&truck_cond);

        }
        pthread_mutex_unlock(&mutex);
    }
}


void* truck (void* arg){
    unsigned long id=(unsigned long)arg;
    pthread_mutex_lock(&mutex);
    int turno=camiones_esperando++;

    while(1){
        printf("Camión %lu esperando turno %d\n", id, turno);

        while(id_ocupado>=0 || turno>turno_actual){

            pthread_cond_wait(&truck_cond,&mutex);
        }

        printf("Camión %lu en el muelle\n", id);
        id_ocupado=id;
        pthread_cond_signal(&worker_cond);
        while(capacidades[id]!=NP){

            pthread_cond_wait(&truck_cond,&mutex);
        }
        printf("Camión %lu sale del muelle\n", id);
        id_ocupado=-1; 
        turno_actual++; 
        capacidades[id]=0;      
        pthread_cond_broadcast(&truck_cond);
        pthread_mutex_unlock(&mutex);
        sleep(rand()%3);
    }
}



int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex\n");
		return -1;
	}
    // || pthread_cond_init(&fumador_cond, NULL) 
    if(pthread_cond_init(&worker_cond, NULL)|| pthread_cond_init(&truck_cond, NULL)){
        perror("error cond\n");
        return -1;
    }
    

	pthread_t* hilos=malloc(5*sizeof(pthread_t)); 

    if(pthread_create(&hilos[0],NULL, worker, (void*)0) || pthread_create(&hilos[1],NULL, worker, (void*)1)	||	pthread_create(&hilos[2],NULL, truck, (void*)0)||pthread_create(&hilos[3],NULL, truck, (void*)1)||pthread_create(&hilos[4],NULL, truck, (void*)2)){

        perror("error al crear al hilo\n");
        return -1;
    }

	for(int i=0;i<5;i++){

        if(pthread_join(hilos[i],NULL)){

            perror("error al esperar al hilo\n");
            return -1;
        }	
    }


	pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&worker_cond);
    pthread_cond_destroy(&truck_cond);
    //pthread_cond_destroy(&productor2_cond);

	free(hilos);

	return 0;
}
