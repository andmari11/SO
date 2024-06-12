#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h> /* for getopt() */
#include <dirent.h>
#include <string.h>



int main(int argc, char* argv[]) {
	FILE* file_output=stdout;
    int opt;
    int n_bytes=0;
    char* output_str;

    while ((opt = getopt(argc, argv, "hn:o:")) != -1)
	{
		switch (opt)
		{
		case 'o':
            output_str=strdup(optarg);
            if ((file_output = fopen(optarg, "w")) == NULL)
                err(2,"The input file %s could not be opened",optarg);
			break;
		case 'n':
			n_bytes = atoi(optarg);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}


    DIR *dir;
    struct dirent *entry;
    char path[1024];
    if ((dir = opendir(".")) == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(path, sizeof(path), "%s/%s", ".", entry->d_name);

        if(strcmp(entry->d_name,output_str)){
            int n=0;
            FILE* file;
            //abrir
            if ((file = fopen(path, "r")) == NULL)
                err(2,"The input file %s could not be opened",path);
            
            
            fwrite(path, 1, strlen(path), file_output);
            fwrite(":\n", 1, 2, file_output);

            unsigned char buffer[1];
            size_t bytes_read;
            //se carga un char (1 byte) de file a buffer
            while ((bytes_read = fread(buffer, 1, 1, file)) > 0 && n<n_bytes) {
                
                //muestra un char (1 byte) por file_output y se comprueba que se impriman todos 
                if (fwrite(buffer, 1, bytes_read, file_output) != bytes_read ) {
                    fclose(file);
                    err(3, "fwrite() failed!!");
                }
                n++;
            }
            fwrite("\n", 1, 1, file_output);
            fclose(file);
        }
    }

    closedir(dir);

    //if(file_output!=stdout){

        fclose(file_output) ; 
   // }
	return 0;
}
