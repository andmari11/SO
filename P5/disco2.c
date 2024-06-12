#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_AFORO 10

typedef struct {
	int id;
	int isvip;
	int ticket;
} client_t;

pthread_mutex_t m;
pthread_cond_t normalClient,vipClient;

int turnoNormales=0;
int turnoVips=0;
int ticketsVips=0;
int ticketsNormales=0;
int huecoslibres=MAX_AFORO;
int vips=0;

void exit_client(int id, int isVip){

    pthread_mutex_lock(&m);
    huecoslibres++;

    if(vips<=0)
        pthread_cond_broadcast(&normalClient);
    else pthread_cond_broadcast(&vipClient);

    pthread_mutex_unlock(&m);

    if(isVip)
        printf("El cliente vip con id %d se ha ido\n",id);
    else printf("El cliente normal con id %d se ha ido\n",id);
}



void dance(int time,int id,int isvip){

    if(isvip)
        printf("Cliente vip con id %d bailando una conga\n",id);
    else printf("Cliente normal con id %d bailando una conga\n",id);
    sleep(time);

    exit_client(id,isvip);
}

void enter_normal_client(int id, int ticket){

    pthread_mutex_lock(&m);

    while(huecoslibres == 0 || turnoNormales!=ticket)
        pthread_cond_wait(&normalClient,&m);

    turnoNormales++;
    huecoslibres--;

    pthread_cond_broadcast(&normalClient);

    pthread_mutex_unlock(&m);

    dance(5,id,0);

}

void enter_vip_client(int id, int ticket){

    pthread_mutex_lock(&m);

    while(huecoslibres == 0 || turnoVips!=ticket)
        pthread_cond_wait(&vipClient,&m);

    turnoVips++;
    huecoslibres--;
    vips--;

    if(vips<=0)
        pthread_cond_broadcast(&normalClient);
    else pthread_cond_broadcast(&vipClient);

    pthread_mutex_unlock(&m);

    dance(5,id,1);

}

void *client(void *arg)
{

	client_t* c = (client_t*) arg;

    pthread_mutex_lock(&m);
    if(c->isvip){
        c->ticket = ticketsVips++;
        vips++;
    }
    else c->ticket = ticketsNormales++;

    pthread_mutex_unlock(&m);

	if (c->isvip)
        enter_vip_client(c->id, c->ticket);
    else
        enter_normal_client(c->id, c->ticket);

}


int main(int argc, void* argv[]){

	//Discoteca de aforo CAPACITY.
	//Si el aforo esta completo los nuevos clientes deberan esperar a que salga algun cliente
	//Si hay esperando clientes vip y clientes normales, se les dara prioridad a los clientes vip.
	//Los clientes entraran de 1 en 1 en orden estricto de llegada segun su grupo

	if(argc != 2) {
		printf("Usage ./disco <clients file name>\n");
		return -1;
	}
    int aforoMaxmo = MAX_AFORO;

    pthread_mutex_init(&m, NULL);
	pthread_cond_init(&normalClient, NULL);
	pthread_cond_init(&vipClient, NULL);


    FILE* fichero = fopen(argv[1],"r");
    char* buffer= malloc(8 * sizeof(char));

    fscanf(fichero,"%s",buffer);
    int numeroClientes = atoi(buffer);

    client_t clientInfo[numeroClientes];
    pthread_t clientes[numeroClientes];

    // while(fscanf(fichero,"%s",buffer)!=EOF){
    //     printf("%s\n",buffer);
    //     clientInfo[i].id=i;
    //     clientInfo[i].isvip = atoi(buffer);
    //     clientInfo[i].ticket=-1;
    //     i++;
    //     if(pthread_create(&clientes[i],NULL,client,&clientInfo[i])!=0){
    //         perror("Error al crear el hilo");
    //         exit(EXIT_FAILURE);
    //     }

    // };


	for(int i = 0; i < numeroClientes ; i++){
		fscanf(fichero,"%s",buffer);
		clientInfo[i].id = i;
		clientInfo[i].isvip = atoi(buffer);
		clientInfo[i].ticket = -1;
        
		if(pthread_create(&clientes[i], NULL, client, &clientInfo[i])!=0){
            perror("Error al crear los hilos");
            exit(EXIT_FAILURE);
        }
	}


    for(int j=0; j< numeroClientes;j++){
        if(pthread_join(clientes[j],NULL)!=0){
            perror("Error al cerrar los hilos");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}