#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

struct options {
	char *progname;
	int recurse;
};

struct options opt;

#define PATH_MAX 100

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


// Abrir el directorio
    DIR *dir = opendir(name);

    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return 1;
    }

    // Leer y mostrar los contenidos del directorio
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    // Cerrar el directorio
    closedir(dir);

}

/* apartado c */
void process_recurse(char *dirname, char *name)
{
	pid_t pid;
	char path[PATH_MAX];


}

/* apartado c */
void list_dir_recurse(char *name)
{
	DIR *d=opendir(name);
	struct dirent *de;
    

    if (d == NULL) {
        perror("Error al abrir el directorio");
        return 1;
    }

    // Leer y mostrar los contenidos del directorio
  

    while ((de = readdir(d)) != NULL) {
        //printf("%s\n", de->d_name);
         if (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
       
	      pid_t child_pid=fork();


		  if(child_pid==0){


          list_dir(de->d_name);
		  exit(1);

		  }else{

			wait(NULL);

		  }





		 }


	
    }

    // Cerrar el directorio
    closedir(d);









}

int main(int argc, char **argv)
{
	char *dirname = ".";
	int o;
	opt.progname = argv[0];
	opt.recurse = 1;


    int option;
	/* Apartado a: procesar opciones con getopt */
    while((option = getopt(argc, argv, "hl:e")) != -1 ) {
		switch(option) {
         case 'h':

		 break;

		 case 'R':
             

			 opt.recurse=1;
           break;

	    

		default:
			exit(EXIT_FAILURE);
		}
	}




	/********************************************/

	if (optind < argc) {
        dirname = argv[optind];
    }

	if (opt.recurse)
		printf("%s:\n", dirname);

	list_dir(dirname);

	if (opt.recurse)
		list_dir_recurse(dirname);

	return 0;
}
