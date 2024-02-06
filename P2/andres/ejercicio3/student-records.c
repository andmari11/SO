#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100


student_t getStudent(FILE* file){

	char line[MAXLEN_LINE_FILE+1];
	char* lineptr;
	char* token;
	token_id_t token_id;
	student_t student;


		fgets(line,MAXLEN_LINE_FILE+1, file);
		lineptr=line;
		token_id=STUDENT_ID_IDX;
		while ((token = strsep(&lineptr, ":")) != NULL)
		{

			switch (token_id)
			{

			case STUDENT_ID_IDX:
				sscanf(token,"%d",&student.student_id);
				break;

			case NIF_IDX:
				strcpy(student.NIF, token);
				break;

			case FIRST_NAME_IDX:

			    student.first_name = (char *)malloc(strlen(token) + 1);

				sscanf(token, "%s",student.first_name);

				break;

			case LAST_NAME_IDX:
				student.last_name = (char *)malloc(strlen(token) + 1);

				strcpy(student.last_name, token);

				break;

			default:

				break;
			}

			token_id++;
		}

		if (token_id!=NR_FIELDS_STUDENT) {
			fprintf(stderr, "error al escanear");
			return student;
		}

	return student;
}

int print_text_file(char *path)
{

	FILE* file;
	if ((file = fopen(path, "r")) == NULL){
		return -2;
	}
	int nEstudiantes;
	fscanf(file, "%d", &nEstudiantes);
	fgetc(file);
	

	student_t student;

	for (int i = 0; i < nEstudiantes; i++)
	{
		student=getStudent(file);
		printf("[Entry #%d]\n", i);

		printf("	student_id=%d\n", student.student_id);
		printf("	NIF=%s\n", student.NIF);
		printf("	first_name=%s\n", student.first_name);
		printf("	last_name=%s\n", student.last_name);
	}

	fclose(file);
	return 1;
}



int print_binary_file(char *path)
{						


	FILE* file;
	if ((file = fopen(path, "r")) == NULL){
								fprintf(stderr, "error input");

		return -2;
	}
	char aux=getc(file);
	int l=0;

	//busca \0 y apunta cuantos caracteres son
	while(aux!=EOF){
		l++;
		aux=getc(file);
	}

	//archivo vacio
	if(l==0){

		printf(" ");

		return 0;
	}

	//coloca el puntero desde current al principio (-length)
	fseek(file, -l, SEEK_CUR);

	//reserva memoria y +1 para colocar el \0
	char* ret=malloc(l+1);

	int a=fread(ret, 1, l+1, file);


	//lee y lo guarda en ret
	if(fread(ret, 1, l+1, file)!=l){

		free(ret);
		return -1;
	}

	//no imprime
	fprintf(stderr, "%s ", ret);
	
	return 0;
}


//no entiendo pq lo puedo abri con cat
int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part B) */

	FILE* input;
	FILE*output;

	//comprueba q se abre bn
	if ((input = fopen(input_file, "r")) == NULL){
						fprintf(stderr, "error input");

		return -2;
	}

	//comprueba q se abre bn

	if ((output = fopen(output_file, "w")) == NULL){
				fprintf(stderr, "error output");

		return -2;
	}
	int nEstudiantes;
	fscanf(input, "%d", &nEstudiantes);
	fgetc(input);
	fwrite(&nEstudiantes, 4,1, output );

	student_t student;

	for (int i = 0; i < nEstudiantes; i++)
	{
		student=getStudent(input);
		fwrite(&student.student_id, sizeof(int), 1, output);
		fwrite(student.NIF, sizeof(char), MAX_CHARS_NIF+1, output);
		fwrite(student.first_name, sizeof(char), strlen(student.first_name) + 1, output);
        fwrite(student.last_name, sizeof(char), strlen(student.last_name) , output);
	}

	fclose(output);
	fclose(input);


	return 1;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h , -i file,-o, file -p, b]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		
		case 'p':

			options.action=PRINT_TEXT_ACT;
			break;

		case 'o':

			options.output_file=optarg;
			options.action=WRITE_BINARY_ACT;
		
			break;
		case 'b':

			options.action=PRINT_BINARY_ACT;
			break;

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
