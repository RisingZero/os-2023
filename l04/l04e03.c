#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

void handler(int sig) {
	if (sig == SIGUSR1) {
		fprintf(stdout, "Father Woke-up\n");
	}

	if (sig == SIGUSR2) {
		fprintf(stdout, "Child Woke-up\n");
	}
}

int main(int argc, char *argv[]) {
	pid_t pid;

	pid = fork();

	if (pid == -1) {
		fprintf(stderr, "Error forking\n");
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		// Parent, prints on SIGUSR1 received
		if (signal(SIGUSR1, handler) == SIG_ERR) {
			fprintf(stderr, "Error setting parent handler\n");
			exit(EXIT_FAILURE);
		}
		
		while(1) {
			sleep(1);
			kill(pid, SIGUSR2);
			pause();
		}
	} else {
		// Child, prints on SIGUSR2 received
		if (signal(SIGUSR2, handler) == SIG_ERR) {
			fprintf(stderr, "Error setting child handler\n");
			exit(EXIT_FAILURE);
		}

		while(1) {
			pause();
			sleep(1);
			kill(getppid(), SIGUSR1);
		}
	}
}
