#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void copy(int fdo, int fdd)
{
	char buffer[512];
	
	size_t bytes_leidos, bytes_escritos;

	while(bytes_leidos=read(fdo, buffer, sizeof(buffer)), bytes_leidos>0){

		bytes_escritos=write(fdd, buffer, bytes_leidos);


		if(bytes_leidos==-1 || bytes_escritos==-1){

			fprintf(stderr, "error");
		} 
	}

	close(fdo);
	close(fdd);

}

int main(int argc, char *argv[])
{
	if(argc!=3){
		
		fprintf(stderr, "Argumentos mal");
		return -1;
	}

	int fdo=open(argv[1], O_RDONLY);
	int fdd=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);

	if(fdo==-1 || fdd==-1){

		return -1;
	}


	copy(fdo, fdd);
	return 0;
}
