#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 128
#define MAX_BUFFER 256

pid_t run_command(const char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/bash", "bash", "-c", command, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void execute_commands_sequentially(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char command[256];
    int cont = 1;

    while (fgets(command, sizeof(command), file)) {
        int status;
        printf("@@ Running command #%d: %c", cont, command);
        pid_t pid = run_command(command);
        waitpid(pid, &status, 0);
        printf("@@ Command #%d terminated (pid: %d, status: %d)\n", cont, cont, WEXITSTATUS(status));
        cont++;
    }

    fclose(file);
}

void execute_commands_concurrently(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    pid_t pids[MAX_COMMANDS];
    int command_number = 0;
    char command[256];
    while (fgets(command, sizeof(command), file) && command_number < MAX_COMMANDS) {
        pids[command_number] = run_command(command);
        printf("@@ Running command #%d: %c", command_number, command);
        command_number++;
    }

    for (int i = 0; i < command_number; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i + 1, pids[i], WEXITSTATUS(status));
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    int sequential = 1;
    char *s_value = NULL;
    int c;
    pid_t pid;

    opterr = 0;

    while ((c = getopt(argc, argv, "x:s:b")) != -1) {
        switch (c) {
            case 'x':
                pid = run_command(optarg);
                waitpid(pid, NULL, 0);
                break;
            case 's':
                s_value = optarg;
                break;
            case 'b':
                sequential = 0;
                break;
            default:
                exit(1);
        }
    }

    if (s_value != NULL) {
        if (sequential) {
            execute_commands_sequentially(s_value);
        } else {
            execute_commands_concurrently(s_value);
        }
    }

    return 0;
}