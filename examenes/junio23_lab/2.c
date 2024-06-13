#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define N 10

int nCorredores_en_posicion=0;
int turno_actual=0;
pthread_mutex_t mutex;
pthread_cond_t condCorredores;


void change_clothes(int id){
    printf("Runner %d is changing clothes\n",id);
    sleep(rand()%5);
}

int get_position(int id){

    pthread_mutex_lock(&mutex);
    int ret=nCorredores_en_posicion++;
    pthread_mutex_unlock(&mutex);

    return ret;
}
void move_position(int id, int pos){
    printf("Runner %d reaches its starting position %d\n",id,pos);
}
void wait_all_ready(int id){
    pthread_mutex_lock(&mutex);
    if(nCorredores_en_posicion<N){
        while(nCorredores_en_posicion<N){
            pthread_cond_wait(&condCorredores,&mutex);
        }
    }
    else{
        pthread_cond_broadcast(&condCorredores);
    }
    pthread_mutex_unlock(&mutex);
}

void wait_my_turn(int id, int turno){
    pthread_mutex_lock(&mutex);
    while(turno>turno_actual){
        pthread_cond_wait(&condCorredores,&mutex);
    }
    pthread_mutex_unlock(&mutex);
}    

void run(int id, int turno){
    printf("Runner %d running from position %d\n",id,turno);
}
void pass_on_relay(int id){
    pthread_mutex_lock(&mutex);
    turno_actual++;
    pthread_cond_broadcast(&condCorredores);
    pthread_mutex_unlock(&mutex);
}

void *thmain(void *arg)
{
    int id = (int)(long long)arg;
    int pos;
    change_clothes(id);
    pos = get_position(id);
    move_position(id, pos);
    wait_all_ready(id);
    wait_my_turn(id, pos);
    run(id, pos);
    pass_on_relay(id);
    return NULL;
}


int main(int argc, char *argv[])
{
    // iniciamos mutex y cond
    if (pthread_mutex_init(&mutex, NULL))
    {
        perror("error mutex");
        return -1;
    }
    if (pthread_cond_init(&condCorredores, NULL)) {
        perror("Error condición");
        return -1;
    }
    pthread_t* hilos=malloc(N*sizeof(pthread_t));

    for(unsigned long i=0;i<N;i++){

        if (pthread_create(&hilos[i], NULL, thmain, (void *)i))
        {
            perror("error al crar hilo");
            return -1;
        }
    }
    for(int i=0;i<N;i++){
        if (pthread_join(hilos[i], NULL))
        {
            perror("error al esperar al hilo");
            return -1;
        }
    }

    
    // destruimos y liberamos memoria
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condCorredores);
    free(hilos);
    return 0;
}
