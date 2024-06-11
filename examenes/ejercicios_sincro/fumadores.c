#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define N 10

pthread_mutex_t mutex;
pthread_cond_t fumador_cond;
pthread_cond_t agente_cond;

char* ingredientes[]={"TABACO", "CERILLAS", "PAPEL"};
bool en_mesa[3]={false,false,false}; 
int n_en_mesa=0;


void* fumador (void* arg){
    unsigned long id=(unsigned long)arg;
    printf("Fumador %lu iniciado\n", id);
    while(1){
        pthread_mutex_lock(&mutex);

        while(en_mesa[id] || n_en_mesa==0){

            pthread_cond_wait(&fumador_cond,&mutex);
        }
        en_mesa[(id+1)%3]=false;
        en_mesa[(id+2)%3]=false;
        n_en_mesa=0;
        printf("Fumador %lu fumando\n", id);

        sleep(1);


        pthread_cond_signal(&agente_cond);
        pthread_mutex_unlock(&mutex);
    }
}


void* agente (void* arg){

    while(1){
        pthread_mutex_lock(&mutex);

        while(n_en_mesa!=0){

            pthread_cond_wait(&agente_cond,&mutex);
        }
        int ingrediente_no_presente= rand()%3;
        en_mesa[0]=ingrediente_no_presente!=0;
        en_mesa[1]=ingrediente_no_presente!=1;
        en_mesa[2]=ingrediente_no_presente!=2;
        n_en_mesa=2;

        printf("En la mesa hay: tabaco(%d), cerillas(%d), papel(%d)\n",en_mesa[0],en_mesa[1],en_mesa[2]);
        pthread_cond_broadcast(&fumador_cond);
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex\n");
		return -1;
	}
    // || pthread_cond_init(&fumador_cond, NULL) 
    if(pthread_cond_init(&agente_cond, NULL)|| pthread_cond_init(&fumador_cond, NULL)){
        perror("error cond\n");
        return -1;
    }
    

	pthread_t* hilos=malloc(4*sizeof(pthread_t)); 

    if(pthread_create(&hilos[0],NULL, agente, NULL) ||	pthread_create(&hilos[1],NULL, fumador, (void*)0)||pthread_create(&hilos[2],NULL, fumador, (void*)1)||pthread_create(&hilos[3],NULL, fumador, (void*)2)){

        perror("error al crear al hilo\n");
        return -1;
    }

	for(int i=0;i<4;i++){

        if(pthread_join(hilos[i],NULL)){

            perror("error al esperar al hilo\n");
            return -1;
        }	
    }


	pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&agente_cond);
    pthread_cond_destroy(&fumador_cond);
    //pthread_cond_destroy(&productor2_cond);

	free(hilos);

	return 0;
}
