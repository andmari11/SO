#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*programa que temporiza la ejecución de un proceso hijo */

void sigalrm_handler(int signo){
	kill(getpid(),SIGKILL);
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
		
		if(sigaction(SIGALRM, &sa, NULL) == -1){
			perror ("Error al establecer el manejo de señales");
			exit(EXIT_FAILURE);
		};

		alarm(5);

		waitpid(child_p, &status,0);

		if((WIFEXITED(status)))
			printf("El hijo ha terminado normalmente\n");
		else if ((WIFSIGNALED(status)))
			printf("El hijo ha sido expulsado por una señal\n");
	}	

	return 0;
}
