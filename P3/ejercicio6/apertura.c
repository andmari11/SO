#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Uso: %s -f <nombre_fichero> [-r] [-w]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = NULL;
    int flags = O_CREAT | O_TRUNC;
    mode_t mode = 0666;

    int opt;
    while ((opt = getopt(argc, argv, "f:rw")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'r':
                flags |= O_RDONLY;
                break;
            case 'w':
                flags |= O_WRONLY;
                break;
            default:
                fprintf(stderr, "Opción no reconocida\n");
                exit(EXIT_FAILURE);
        }
    }

    if (!filename || !(flags & (O_RDONLY | O_WRONLY))) {
        fprintf(stderr, "Debes proporcionar un nombre de archivo y al menos un modo de apertura (-r o -w)\n");
        exit(EXIT_FAILURE);
    }

    int fd = open(filename, flags, mode);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char buffer[100] = "Hola, mundo!";
    if (write(fd, buffer, sizeof(buffer)) == -1) {
        perror("Error al escribir en el archivo");
        exit(EXIT_FAILURE);
    }

    lseek(fd, 0, SEEK_SET);  // Volver al inicio del archivo

    char read_buffer[100];
    if (read(fd, read_buffer, sizeof(read_buffer)) == -1) {
        perror("Error al leer del archivo");
        exit(EXIT_FAILURE);
    }

    printf("Contenido leído: %s\n", read_buffer);

    if (close(fd) == -1) {
        perror("Error al cerrar el archivo");
        exit(EXIT_FAILURE);
    }

    return 0;
}
