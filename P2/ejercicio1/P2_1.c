#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;

	//comprueba cuantos argumentos
	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	//abrir
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);
    
    unsigned char buffer[1];
    size_t bytes_read;
    
	//se carga un char (1 byte) de file a buffer
    while ((bytes_read = fread(buffer, 1, 1, file)) > 0) {
		
        //muestra un char (1 byte) por stdout y se comprueba que se impriman todos 
        if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read) {
            fclose(file);
            err(3, "fwrite() failed!!");
        }
    }
	
	fclose(file);
	return 0;
}
