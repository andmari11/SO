#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*programa que temporiza la ejecución de un proceso hijo */
/* Manejador de la señal SIGALRM */
void sigalrm_handler(int signo) {
    // Enviar una señal SIGKILL al proceso hijo
    kill(getpid(), SIGKILL);
}
int main(int argc, char **argv)
{
	if(argc < 2){
		fprintf(stderr, "Usage: %s command [args]", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid_t child_p = fork();
	int status;

	if(child_p == -1){
		perror("Error: fork()");
		exit(1);
	}else if (child_p == 0){
		if(execvp(argv[1], argv+1)==1){
			perror("Error: execvp");
			exit(1);
		}		
		
	}else{
		struct sigaction sa;
		sa.sa_handler = sigalrm_handler;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);

		if(sigaction(SIGALRM, &sa, NULL) == -1){
			perror ("Error al establecer el manejo de señales");
			exit(EXIT_FAILURE);
		};

		alarm(1);

		//waitpid(child_p, &status,0);
        wait(&status);

        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó de forma normal con estado %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo fue terminado por la señal %d.\n", WTERMSIG(status));
        }
	}	

	return 0;
}
