#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 512

void copy(int fdo, int fdd)
{
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

void copy_regular(char *orig, char *dest)
{
    int fd_origen, fd_destino;

    // Abrir el archivo origen en modo lectura
    fd_origen = open(orig, O_RDONLY);
    if (fd_origen == -1) {
        perror("Error al abrir el fichero origen");
        exit(EXIT_FAILURE);
    }

    // Abrir o crear el archivo destino en modo escritura, truncando si ya existe
    fd_destino = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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

    printf("Copia de fichero regular realizada con éxito.\n");

}

void copy_link(char *orig, char *dest)
{
	char* buffer;
    size_t len;

	struct stat statbuf;
	if (lstat(orig, &statbuf) == -1){

		perror("Error al obtener información del enlace simbólico");
		exit(EXIT_FAILURE);
	}

	len = statbuf.st_size; //Tamaño del enlace sin ser nulo
	buffer = (char *) malloc(len + 1);
    len = readlink(orig, buffer, len);
    if (len == -1) {
        perror("Error al leer el enlace simbólico");
		free(buffer);
        exit(EXIT_FAILURE);
    }

    buffer[len] = '\0';

    if (symlink(buffer, dest) == -1) {
        perror("Error al crear el enlace simbólico");
        exit(EXIT_FAILURE);
    }

	free(buffer);
    printf("Creación de enlace simbólico realizada con éxito.\n");
}

int main(int argc, char *argv[])
{
if (argc != 3) {
        fprintf(stderr, "Uso: %s <fichero_origen> <fichero_destino>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (lstat(argv[1], &statbuf) == -1) {
        perror("Error al obtener información del fichero origen");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(statbuf.st_mode)) {
        copy_regular(argv[1], argv[2]);
    } else if (S_ISLNK(statbuf.st_mode)) {
        copy_link(argv[1], argv[2]);
    } else {
        fprintf(stderr, "El tipo de fichero no es soportado.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
