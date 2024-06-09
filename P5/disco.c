#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

int turno_actual_vip=0;
int vip_fila=0;

//por q turno de la fila normal, cuantos normales han entrado
int turno_actual=0;
//cuantos hay en la fila de no vip
int fila_normal=0;


//clientes totales
int nClientes=0;
//cuantos vips hay en total
int nVips=0;

//cuantos clientes hay dentro
int clientesDentro=0;

pthread_mutex_t mutex;
pthread_cond_t condNormal, condVip;

struct tCliente{

	int isvip, id;
	pthread_t thread;

};


void enter_normal_client(int id);

void enter_vip_client(int id);

void dance(int id, int isvip);

void disco_exit(int id, int isvip);

void *client(void *arg);

int main(int argc, char *argv[])
{	
	FILE *file;

	//iniciamos mutex y cond
	if(pthread_mutex_init(&mutex, NULL)){
		perror("error mutex");
		return -1;
	}
	if(pthread_cond_init(&condNormal, NULL) || pthread_cond_init(&condVip, NULL)){
		perror("error cond");
		return -1;
	}

	//abrimos el fichero
	file= fopen("ejemplo.txt", "r");
	if(file==NULL){

		perror("error fichero");
		return -1;
	}

	//miramos el numero N de clientes
	if (fscanf(file, "%d", &nClientes) == EOF) {
        fprintf(stderr, "Error reading file\n");
        fclose(file);
        return EXIT_FAILURE;
    }

	//reservamos
	struct tCliente* clientes=malloc(nClientes*sizeof(struct tCliente));

	//escaneamos que clientes son vip
	for(int i=0;i<nClientes;i++){

		fscanf(file, "%d", &clientes[i].isvip);
		clientes[i].id=i;

		nVips+=clientes[i].isvip;
	}
	fclose(file);

	//creamos un hilo para cada cliente
	for(int i=0;i<nClientes;i++){

		if(pthread_create(&clientes[i].thread, NULL, client, (void *)&clientes[i])){

			perror("error al crar hilo");
			return -1;

		}
	}
	//llamamos de vuelta a los clientes
	for(int i=0;i<nClientes;i++){

		if(pthread_join(clientes[i].thread, NULL)){

			perror("error al esperar al hilo");
			return -1;
		}
	}


	//destruimos y liberamos memoria
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condNormal);
	pthread_cond_destroy(&condVip);

	free(clientes);
	return 0;
}

void enter_normal_client(int id)
{
	//bloqueamos al principio
	pthread_mutex_lock(&mutex);
	//tomamos el turno
	int turno=fila_normal++;

	//esperamos a q pasen los vips y a que haya hueco
	while(clientesDentro==CAPACITY || nVips>0){

		pthread_cond_wait(&condVip, &mutex);
	}

	//esperamos a q haya hueco y sea nuestro turno
	while(clientesDentro==CAPACITY || turno_actual<turno){

		pthread_cond_wait(&condNormal, &mutex);
	}
	turno_actual++;
	clientesDentro++;

	//desbloqueamos
	pthread_mutex_unlock(&mutex);

}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);
	int turno=vip_fila++;

	while(clientesDentro==CAPACITY||turno_actual_vip<turno){

		pthread_cond_wait(&condVip, &mutex);
	}
	turno_actual_vip++;
	clientesDentro++;

	nVips--;
	if(nVips==0){

		printf("No vips waiting\n");
		pthread_cond_broadcast(&condVip);
	}
	pthread_mutex_unlock(&mutex);	

}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mutex);

	printf("- cliente[%2d] (%s) se va\n", id, VIPSTR(isvip));
	pthread_cond_broadcast(&condNormal);
	clientesDentro--;

	pthread_mutex_unlock(&mutex);
}

void *client(void *arg)
{

	struct tCliente *cliente=(struct tCliente*)arg;

	printf("+ Cliente[%d][%s] ha sido creado\n", cliente->id, VIPSTR(cliente->isvip));
	//while(FOREVER){
	if(cliente->isvip==1){

		enter_vip_client(cliente->id);
	}
	else{
		enter_normal_client(cliente->id);
	}
	dance(cliente->id,cliente->isvip);

	disco_exit(cliente->id, cliente->isvip);
	//}
}