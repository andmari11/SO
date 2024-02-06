#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[]) {
    int opt;
    int fd;
    int n = 0;
    int end_flag = 0;

    // Parsear argumentos con getopt
    while ((opt = getopt(argc, argv, "n:e")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'e':
                end_flag = 1;
                break;
            default: // Si se encuentra un argumento no reconocido
                fprintf(stderr, "Uso: %s [-n N] [-e] archivo\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Comprobar que se ha proporcionado un nombre de archivo
    if (optind >= argc) {
        fprintf(stderr, "Se esperaba un nombre de archivo.\n");
        exit(EXIT_FAILURE);
    }

    // Abrir el archivo
    if ((fd = open(argv[optind], O_RDONLY)) == -1) {
        perror("No se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Posicionar el descriptor de archivo usando lseek
    off_t position;
    if (end_flag == 1) {
        position = lseek(fd, -n - 1, SEEK_END);
        if (position == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
    } else {
        position = lseek(fd, n, SEEK_SET);
        if (position == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
    }

    // Leer y escribir el archivo
    char buffer;
    size_t bytes_read;
    while ((bytes_read = read(fd, &buffer, 1)) > 0) {
        if (write(STDOUT_FILENO, &buffer, 1) != 1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Cerrar el archivo
    close(fd);

    return 0;
}