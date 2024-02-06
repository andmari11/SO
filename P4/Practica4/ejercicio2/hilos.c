
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


typedef struct{
int numero_hilo;
char prioritario;

}DatosHilo;

void *thread_usuario(void *arg)
{

DatosHilo* a=(struct DatosHilo*) arg;

printf("%i%c",a->numero_hilo,a->prioritario);


}

int main(int argc, char* argv[])
{
	

   pthread_t hilos[5];



   int act_hilo;
	DatosHilo* Hilo= malloc(sizeof(DatosHilo));


    DatosHilo arrHilo[5];

    for(int i=0;i<5;i++){
     Hilo->numero_hilo=i;
	  
	  if(i%2==0){

      Hilo->prioritario='P';
	  }else{
      Hilo->prioritario='N';

	  }
      arrHilo[i]=*Hilo;


    }    


     for(int i=0;i<5;i++){
      
      



	  if(pthread_create(&hilos[i],NULL,thread_usuario,&arrHilo[i])!=0){


	  }
	  
	  
    }
	

    for (int i = 0; i < 5; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("Error al esperar el hilo");
            exit(1);
        }
	}
	

    free(Hilo);
	
printf("\n");

	return 0;
}



