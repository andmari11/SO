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
            inicio = 1;
        } else if (inicio) {
            if (argv != NULL) {
                argv[nArgs] = &args[i];
            }
            nArgs++;
            inicio = 0;
        }
        i++;
    }
    return nArgs;
}

int main(int argc, char *argv[]) {
    // Verificar que se proporcionó un argumento
    if (argc != 2) {
        fprintf(stderr, "Uso: %s archivo_de_entrada\n", argv[0]);
        return 1;
    }

    // Abrir el archivo de entrada
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Buffer para leer cada línea
    char line[256];

    // Procesar el archivo línea por línea
    while (fgets(line, sizeof(line), file) != NULL) {
        // Eliminar el salto de línea si existe
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        // Contar el número de argumentos y establecerlos
        int numArgs = setargs(line, NULL);
        char **cargv = malloc((numArgs + 1) * sizeof(char *));
        setargs(line, cargv);

        // Crear un nuevo proceso
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            return 1;
        } else if (pid == 0) {  // Proceso hijo
            // Ejecutar el comando
            execvp(cargv[0], cargv);
            // Si execvp devuelve, ocurrió un error
            perror("Error al ejecutar el comando");
            exit(1);
        } else {  // Proceso padre
            // Esperar a que el proceso hijo termine
            wait(NULL);
        }

        // Liberar memoria
        free(cargv);
    }

    // Cerrar el archivo
    fclose(file);

    return 0;
}
