#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/* Manejador de la señal SIGALRM */
void sigalrm_handler(int signo) {
    // Enviar una señal SIGKILL al proceso hijo
    kill(getpid(), SIGKILL);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando a ejecutar>\n", argv[0]);
        return 1;
    }

    pid_t pid;
    int status;
    struct sigaction sa;

    pid = fork();

    if (pid == -1) {
        perror("Error al crear el proceso hijo");
        return 1;
    }

    if (pid == 0) {
        // Proceso hijo
        char *cmd = argv[1];
        char **cmd_args = &argv[1];

        // Cambiar el ejecutable del proceso hijo y ejecutar el comando
        if (execvp(cmd, cmd_args) == -1) {
            perror("Error al ejecutar el comando");
            exit(1);
        }
    } else {
        // Proceso padre
        sa.sa_handler = sigalrm_handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);

        // Establecer el manejador de señal SIGALRM
        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            perror("Error al establecer el manejador de SIGALRM");
            return 1;
        }

        // Programar una alarma para enviar la señal SIGALRM después de 5 segundos
        sleep(5);

        // Esperar a que el proceso hijo termine
        wait(&status);

        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó de forma normal con estado %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo fue terminado por la señal %d.\n", WTERMSIG(status));
        }
    }

    return 0;
}

