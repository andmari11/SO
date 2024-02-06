#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>


const unsigned char SALTO = '\n';
const unsigned char FIN = '\0';


int main(int argc, char* argv[]){

	FILE* file=NULL;
	char aux=SALTO;

	if (argc<2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "w")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);


	for(int i=2;i<argc;i++){

				
		if (fwrite(argv[i], 1, strlen(argv[i]), file)!=strlen(argv[i])){
			fclose(file);
			err(3,"error al escribir");
		}

		if(i+1==argc){
			aux=FIN;
		}

		if (fwrite(&aux, 1, 1, file)!=1){
			fclose(file);
			err(3,"error al escribir");
		}



	}


	fclose(file);

	return 0;
}
