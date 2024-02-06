#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 512

void copy(int fdo, int fdd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_leidos, bytes_escritos;

    // Leer y escribir en bloques de 512 bytes
    while ((bytes_leidos = read(fdo, buffer, BUFFER_SIZE)) > 0) {
        bytes_escritos = write(fdd, buffer, bytes_leidos);
        if (bytes_escritos != bytes_leidos) {
            perror("Error al escribir en el fichero destino");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_leidos == -1) {
        perror("Error al leer el fichero origen");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    // Verificar que se proporcionen los argumentos correctos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <fichero_origen> <fichero_destino>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd_origen, fd_destino;

    // Abrir el archivo origen en modo lectura
    fd_origen = open(argv[1], O_RDONLY);
    if (fd_origen == -1) {
        perror("Error al abrir el fichero origen");
        exit(EXIT_FAILURE);
    }

    // Abrir o crear el archivo destino en modo escritura, truncando si ya existe
    fd_destino = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_destino == -1) {
        perror("Error al abrir/crear el fichero destino");
        exit(EXIT_FAILURE);
    }

    // Llamar a la función copy para copiar el contenido
    copy(fd_origen, fd_destino);

    // Cerrar archivos
    if (close(fd_origen) == -1) {
        perror("Error al cerrar el fichero origen");
        exit(EXIT_FAILURE);
    }
    if (close(fd_destino) == -1) {
        perror("Error al cerrar el fichero destino");
        exit(EXIT_FAILURE);
    }

    printf("Copia realizada con éxito.\n");

    return 0;
}