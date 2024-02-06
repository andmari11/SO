#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>


struct options {
	char *progname;
	int recurse;
};

struct options opt;

void usage(void)
{
	printf("%s [options] [dirname]\n\n", opt.progname);
	printf("lists the contents of dirname (default .)\n");
	printf("options:\n"
		"\t-h:\tshow this help\n"
		"\t-R:\trecursive\n"
	);
}

/* apartado b */
void list_dir(char *name)
{
	DIR *directorio = opendir(name);

	if (directorio == NULL)
	{
		perror("opendir");
		exit(-1);
	}
	struct dirent *entrada;

	while (entrada = readdir(directorio), entrada != NULL)
	{
		if (strcmp(entrada->d_name, ".") && strcmp(entrada->d_name, ".."))
		{
			struct stat file_stat;
			if (lstat(entrada->d_name, &file_stat) == -1)
			{
				perror("lstat");
				exit(-1);
			}
			printf("%s\n", entrada->d_name);

		}
	}
	closedir(directorio);
}

/* apartado c */
void process_recurse(char *dirname, char *name)
{
	pid_t pid;
	char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/%s", dirname, name);
    pid_t child_pid = fork();
	if (child_pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if(child_pid>=0){

		list_dir_recurse(path);
	}
	else{

		wait(NULL);
	}
}

/* apartado c */
void list_dir_recurse(char *name)
{
	DIR *d=opendir(name);
	struct dirent *de;

	if (d == NULL)
	{
		perror("opendir");
		exit(-1);
	}
	printf("%s:\n", name);
	while (de = readdir(d), de != NULL)
	{
		
			struct stat file_stat;
			if (lstat(de->d_name, &file_stat) == -1)
			{
				perror("lstat");
				exit(-1);
			}

			if (S_ISREG(file_stat.st_mode) || !strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			{

				printf("%s\n", de->d_name);
			}
			else{
				process_recurse(name, de->d_name);
			} 

		
	}
	closedir(d);

}

int main(int argc, char **argv)
{
	char *dirname = ".";
	int o;
	opt.progname = argv[0];
	opt.recurse = 0;

	/* Apartado a: procesar opciones con getopt */

	while ((o = getopt(argc, argv, "hR")) != -1)
	{
		switch (o)
		{

			case 'R':
				opt.recurse=1;

			break;
		default:
			fprintf(stderr, "Uso: %s [-n N] [-e] archivo\n", argv[0]);
			exit(-1);
		}
	}




/*
	if(argv[optind][0]!='\0'){

		dirname=argv[optind];
	}
	printf( dirname);


	if(opt.recurse==0){

		list_dir(dirname);
	}
	else{

		list_dir_recurse(dirname);
	}

*/
	/********************************************/

	if (opt.recurse==0){
		printf("%s:\n", dirname);
		list_dir(dirname);
	}

	if (opt.recurse>0)
		list_dir_recurse(dirname);

	return 0;
}
