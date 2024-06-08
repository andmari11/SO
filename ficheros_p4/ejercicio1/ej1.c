#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <getopt.h>

#define MAX_PATH_LENGTH 4096

void list_dir(const char *dir_path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                printf("%s\n", dir->d_name);
            }
        }
        closedir(d);
    }
}

void list_dir_recursive(const char *dir_path) {
    list_dir(dir_path);
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                char subdir_path[MAX_PATH_LENGTH];
                snprintf(subdir_path, sizeof(subdir_path), "%s/%s", dir_path, dir->d_name);
                pid_t pid = fork();
                if (pid == 0) {
					printf("\n%s:\n",subdir_path);
					list_dir_recursive(subdir_path);
                    exit(EXIT_SUCCESS);
                } else if (pid < 0) {

                    fprintf(stderr, "Error creating child process.\n");
                } else {
                    wait(NULL);
                }
            }
        }
        closedir(d);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    bool recursive = false;

    // Parse command line options
    while ((opt = getopt(argc, argv, "hR")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-h] [-R] [directory]\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'R':
                recursive = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-R] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *dir_path = (optind < argc) ? argv[optind] : ".";

    // Print directory path
    printf("%s:\n", dir_path);

    if (recursive) {
        list_dir_recursive(dir_path);
    } else {
        list_dir(dir_path);
    }

    return EXIT_SUCCESS;
}
