#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void copy(int fdo, int fdd);
void copy_regular(int fdo, int fdd);
void copy_link(const char *fdo, const char *fdd);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <archivo_origen> <archivo_destino>\n", argv[0]);
        return 1;
    }

    int fdo = open(argv[1], O_RDONLY);
    int fdd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fdo == -1 || fdd == -1)
    {
        perror("Error al abrir archivos");
        return 1;
    }

    copy(fdo, fdd);

    close(fdo);
    close(fdd);

    return 0;
}

void copy(int fdo, int fdd)
{
    struct stat file_info;

    if (fdo == -1 || fdd == -1)
    {
        return;
    }

    if (fstat(fdo, &file_info) != 0)
    {
        perror("Error al obtener información del archivo de origen");
        return;
    }

    if (S_ISREG(file_info.st_mode))
    {
        copy_regular(fdo, fdd);
    }
    else if (S_ISLNK(file_info.st_mode))
    {
        copy_link(fdo, fdd);
    }
}

void copy_regular(int fdo, int fdd)
{
    char buffer[512];
    ssize_t bytes_leidos, bytes_escritos;


    while ((bytes_leidos = read(fdo, buffer, sizeof(buffer))) > 0)
    {
        bytes_escritos = write(fdd, buffer, bytes_leidos);

        if (bytes_escritos == -1)
        {
            perror("Error al escribir en el archivo de destino");
            return;
        }
    }
}

void copy_link(const char *fdo, const char *fdd)
{
   struct stat fdo_stat;

    // Utilizamos lstat para obtener información sobre el enlace
    if(lstat(fdo, &fdo_stat)==-1){
        perror("lstat");
        return;
    }


    //destino del enlace simbólico
    char link[fdo_stat.st_size+1];
    ssize_t link_size= readlink(fdo, link, sizeof(link));

    if(link_size==-1){
        perror("readlink");
        return;
    }
    link[link_size]='\0';

    //crear nuevo enlace
    if (symlink(link, fdd) == -1) {
        
        perror("symlink");
        return;        
    }
}
