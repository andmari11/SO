#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int setargs(char* args, char* argv[]) {
    int i = 0;
    int nArgs = 0;
    int inicio = 1;

    while (args[i] != '\0') {
        if (isspace(args[i])) {
            if (argv != NULL) {
                args[i] = '\0';
            }
            nArgs++;
            inicio = 1;
        } else if (inicio) {
            if (argv != NULL) {
                argv[nArgs] = &args[i];
            }
            inicio = 0;
        }
        i++;
    }
    if (argv != NULL) {
        argv[nArgs] = NULL; 
    }
    return nArgs;
}

int main(int argc, char **argv) {
    char line[256];
    FILE* file;

    if (argc != 2) {
        printf("faltan args\n");
        return 1;
    }

    if ((file = fopen(argv[1], "r")) == NULL) {
        printf("The input file %s could not be opened\n", argv[1]);
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char* args = strdup(line);
        int nArgs = setargs(args, NULL);
        char** cmd_args = malloc((nArgs + 1) * sizeof(char*));
        setargs(args, cmd_args);

        // for (int i = 0; i < nArgs; i++) {
        //     printf("%s, ", cmd_args[i]);
        // }

        pid_t pid = fork();
        if (pid == -1) {
            perror("Error al crear el proceso hijo\n");
            free(args);
            free(cmd_args);
            fclose(file);
            return 1;
        } else if (pid == 0) {
            if (execvp(cmd_args[0], cmd_args) < 0) {
                perror("Error al ejecutar el comando");
                free(args);
                free(cmd_args);
                exit(0);
            }
            exit(0);

        } else {
            wait(NULL);
        }

        free(args);
        free(cmd_args);
    }

    fclose(file);
    return 0;
}
