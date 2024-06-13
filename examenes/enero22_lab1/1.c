#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define MAX_COMMANDS 128
#define MAX_BUFFER 256

int main(int argc, char *argv[])
{
    int opt;
    pid_t pid;
    char *path_input = NULL;
    char *path_output = NULL;
    FILE *output = stdout;
    bool recursive = false;

    while ((opt = getopt(argc, argv, "i:ro:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            path_input = strdup(optarg);
            break;
        case 'o':
            path_output = strdup(optarg);
            break;
        case 'r':
            recursive = true;
            break;
        default:
            exit(1);
        }
    }
    if (path_output != NULL)
    {
        output = fopen(path_output, "w");
        if (output == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }
    if (!recursive)
    {

        FILE *file = fopen(path_input, "r");
        if (file == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        char aux[100];
        fgets(aux, 100, file);
        int bytes_read, n_bytes = atoi(aux);
        int n = 0;
        while (n < n_bytes)
        {
            strcpy(aux, "");
            bytes_read = fread(aux, 1, 1, file);
            if (fwrite(aux, 1, bytes_read, output) != bytes_read)
            {
                fclose(file);
                perror("fwrite() failed!!");
            }
            n++;
        }
    }
    else
    {
        int offset = 0;
        DIR *d;
        pid_t pid[10];
        int pids_n=0;
        struct dirent *dir;
        d = opendir(path_input);
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (dir->d_type != DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
                {
                    char subdir_path[100];
                    snprintf(subdir_path, sizeof(subdir_path), "%s/%s", path_input, dir->d_name);
                    printf("%s:\n", subdir_path);
                    FILE *file = fopen(subdir_path, "r");
                    if (file == NULL)
                    {
                        perror("fopen");
                        exit(EXIT_FAILURE);
                    }
                    fseek(output, offset, SEEK_SET);

                    char aux[100];
                    fgets(aux, 100, file);
                    int n_bytes = atoi(aux);
                    offset += n_bytes;
                    pid[pids_n] = fork();
                    if (pid[pids_n++] == 0)
                    {
                        int bytes_read;
                        int n = 0;
                        while (n < n_bytes)
                        {
                            strcpy(aux, "");
                            bytes_read = fread(aux, 1, 1, file);
                            if (fwrite(aux, 1, bytes_read, output) != bytes_read)
                            {
                                fclose(file);
                                perror("fwrite() failed!!");
                            }
                            n++;
                        }
                        printf("\n", subdir_path);
                        return 0;
                    }
                    else if (pid < 0)
                    {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                    // else{
                    //     wait(NULL);
                    // }
                    
                }
            }
            for (int i = 0; i < pids_n; i++)
            {
                int status;
                waitpid(pid[i], &status, 0);
                printf("(pid: %d, status: %d)\n", i + 1, pid[i], WEXITSTATUS(status));
            }
            closedir(d);
        }
    }

    return 0;
}
