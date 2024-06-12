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
#define M 5
#define OUTPUT "output.txt"
pthread_mutex_t mutex;
pthread_cond_t condHilos[N];
FILE* file;    
int turno=0;

//b
// void* action(void* arg){
//     unsigned long id = (unsigned long)arg;
//     FILE* file;    
//     char* buffer=malloc(1);
//     snprintf(buffer, sizeof(buffer), "%d", id);

// 	if ((file = fopen(OUTPUT, "w")) == NULL){
// 		printf("The input file %s could not be opened",OUTPUT);
//         exit(1);
//     }
//     fseek(file,5*id,SEEK_SET);
//     for(int i=0;i<M;i++){
//         fwrite(buffer,1,1,file);
//     }
// }

void* action(void* arg){
    unsigned long id = (unsigned long)arg;
    char* buffer=malloc(1);

    snprintf(buffer, sizeof(buffer), "%d", id);
    pthread_mutex_lock(&mutex);
    while(turno!=id){

        pthread_cond_wait(&condHilos[id],&mutex);
    }
    for(int i=0;i<M;i++){
        
        fwrite(buffer,1,1,file);
    }
    free(buffer);
    pthread_cond_signal(&condHilos[++turno]);
    pthread_mutex_unlock(&mutex);

}


int main(int argc, char *argv[])
{
    // iniciamos mutex y cond
    if (pthread_mutex_init(&mutex, NULL))
    {
        perror("error mutex");
        return -1;
    }
	if ((file = fopen(OUTPUT, "w")) == NULL){
		printf("The input file %s could not be opened",OUTPUT);
        exit(1);
    }
    pthread_t* hilos=malloc(N*sizeof(pthread_t));
    for(unsigned long i=0;i<N;i++){
        if (pthread_cond_init(&condHilos[i], NULL))
        {
            perror("error cond");
            return -1;
        }
    }
    for(unsigned long i=0;i<N;i++){

        if (pthread_create(&hilos[i], NULL, action, (void *)i))
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
    for(int i=0;i<N;i++)
        pthread_cond_destroy(&condHilos[i]);
    // /free(hilos);
    return 0;
}
