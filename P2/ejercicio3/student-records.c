#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

char *loadstr(FILE *file)
{
	int l = 2;

	if(getc(file) == EOF) return NULL;

	//archivo vacio??
	while(getc(file) != '\0'){
		l++;
	}

	if(l > 2){
		//l = l+1;
		char* word = malloc(l);
		fseek(file, -l, SEEK_CUR);

		fread(word, sizeof(char), l, file);
		word[l] = '\0';
		
		return word;	
	}
	
	return NULL;
}

void printStudent(student_t st, int i){
	printf("[Entry #%d]\n", i);
	printf("	student_id=%d\n", st.student_id);
	printf("	NIF=%s\n", st.NIF);
	printf("	first_name=%s\n", st.first_name);
	printf("	last_name=%s\n", st.last_name);
}

static inline char* clone_string(char* original)
{
	char* copy;
	copy=malloc(strlen(original)+1);
	strcpy(copy,original);
	return copy;
}

int print_text_file(char *path)
{
	/*(part A) */

	FILE* f = NULL;
	token_id_t token;
	char* info;
	char* line = malloc(MAXLEN_LINE_FILE+1);
	char*lineInfo;


    //abrir fichero
	if((f = fopen(path, "r")) == NULL){
		fprintf(stderr, "File %s could not be open", path);
		exit(1);
	}

	//cuantos alumnos
	fgets(line, MAXLEN_LINE_FILE+1, f);
	int nRegistros = atoi(line);

	for(int i = 0; i < nRegistros; i++){

		fgets(line, MAXLEN_LINE_FILE+1, f);
		token = STUDENT_ID_IDX;
		student_t st;
		lineInfo = line;

		//strsep devuelve el string quitando el :
		while((info = strsep(&lineInfo, ":")) != NULL){

			switch (token)
			{
			case STUDENT_ID_IDX:
				st.student_id = atoi(info);
				break;
			
			case NIF_IDX:
				strcpy(st.NIF, info);
				break;

			case FIRST_NAME_IDX:
				st.first_name = clone_string(info);
				break;

			case LAST_NAME_IDX:
				st.last_name = clone_string(info);
				break;

			case NR_FIELDS_STUDENT:
			break;
			}


			//soguiente campo
			token++;
		}

		if (token != NR_FIELDS_STUDENT) {
			printf("Could not process all tokens\n");
			exit(1);
		}

		printStudent(st,i);
	}

	fclose(f);

	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */

	FILE* f = NULL;

	if((f = fopen(path, "rb"))== NULL){
		fprintf(stderr, "Input File %s could not be open", path);
		exit(1);
	}

	char* word;
	int aux;
	fread(&aux, sizeof(int), 1, f);
	

	for(int i = 0; i < aux;i++){

		printf("[Entry #%d]\n", i);
		int n;
		fread(&n, sizeof(int), 1, f);
		printf("	student_id=%d\n", n);
		word = loadstr(f);
		printf("	NIF=%s\n", word);
		word = loadstr(f);
		printf("	first_name=%s\n", word);
		word = loadstr(f);
		printf("	last_name=%s\n", word);	

	}

	fclose(f);

	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{

	FILE* input = NULL;
	FILE* output = NULL;
	char* line = malloc(MAXLEN_LINE_FILE+1);
	char* lineInfo;
	char*info;
	char*word;
	token_id_t token;

	if((input = fopen(input_file, "r"))== NULL){
		fprintf(stderr, "Input File %s could not be open", input_file);
		exit(1);
	}

	if((output = fopen(output_file, "wb"))== NULL){
		fprintf(stderr, "Output File %s could not be open", output_file);
		exit(1);
	}

	fgets(line, MAXLEN_LINE_FILE, input);
	int k = atoi(line);
	fwrite(&k, sizeof(int), 1, output);

	for(int i = 0; i < k; i++){

		fgets(line, MAXLEN_LINE_FILE, input);
		token = STUDENT_ID_IDX;
		student_t st;
		lineInfo = line;

		while((info = strsep(&lineInfo, ":")) != NULL){

			switch (token)
			{
			case STUDENT_ID_IDX:
				st.student_id = atoi(info);
				fwrite(&st.student_id, sizeof(int), 1, output);
				break;
			
			case NIF_IDX:
				strcpy(st.NIF, info);
				st.NIF[strlen(st.NIF)+1] = '\0';
				fwrite(st.NIF, sizeof(char), strlen(st.NIF)+1, output);
				break;

			case FIRST_NAME_IDX:
				st.first_name = clone_string(info);
				word = st.first_name + '\0';
				fwrite(word, sizeof(char), strlen(word)+1, output);
				break;

			case LAST_NAME_IDX:
				st.last_name = clone_string(info);
				st.last_name[strlen(st.last_name)-1] = '\0'; // es el -1 porque al ser el último, fgets incluye el caracter \n que se representa 0a
				word = st.last_name;
				fwrite(word, sizeof(char), strlen(word)+1, output);
				break;

			case NR_FIELDS_STUDENT:
			break;
			}
			
			token++;
		}

		if (token != NR_FIELDS_STUDENT) {
			printf("Could not process all tokens\n");
			exit(1);
		}

	}

	fclose(input);
	fclose(output);

	return 0;
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
			fprintf(stderr, "Usage: %s [ -h ] | [-i <input_file>] | [-p] | [-o <output_file>] | [-b] \n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = optarg;
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
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
