#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>


const unsigned char SALTO = '.';
const unsigned char FIN = '\0';


/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{

	char aux=getc(file);
	int l=0;

	//busca \0 y apunta cuantos caracteres son
	while(aux!=FIN){

		l++;
		aux=getc(file);
	}

	//archivo vacio
	if(l==0){

		return "";
	}

	//coloca el puntero desde current al principio (-length)
	fseek(file, -l, SEEK_CUR);

	//reserva memoria y +1 para colocar el \0
	char* ret=malloc(l+1);

	//lee y lo guarda en ret
	if(fread(ret, 1, l, file)!=l){

		free(ret);
		err(3, "error al escribir");
		return NULL;
	}

	//coloca \0
	ret[l]=FIN;

	return ret;	
}

int main(int argc, char *argv[])
{

	FILE* file=NULL;

	if (argc<2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);



	char* ret=loadstr(file);
	printf(ret);
	free(ret);

	fclose(file);
	return 0;
}
