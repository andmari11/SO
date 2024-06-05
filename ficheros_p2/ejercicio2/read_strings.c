#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

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

	while(aux!=EOF){

		l++;
		aux=getc(file);
	}

	if(l==0){

		return "";
	}
	
	fseek(file, 0, SEEK_SET);
	char* ret=malloc(l+1);

	if(fread(ret, 1, l, file)!=l){

		free(ret);
		err(3, "error al escribir");
		return NULL;
	}

	ret[l]='\0';

	return ret;	
}

int main(int argc, char *argv[])
{
	if(argc==2){
		FILE* file=NULL;
		/* Open file */
		if ((file = fopen(argv[1], "r")) == NULL)
			err(2,"The input file %s could not be opened",argv[1]);
		else
			fprintf(stderr, loadstr(file));
	}
	return 0;
}
