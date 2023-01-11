#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	
	int p_to_c[2];
	int c_to_p[2];
	int n = 0;
	pid_t pid;
	char sig;

	pipe(p_to_c);
	pipe(c_to_p);

	pid = fork();
	if (pid > 0) {
		close(p_to_c[0]);
		close(c_to_p[1]);

		while (1) {
			n++;
			if (n > 20)
				fprintf(stdout, "Father quitting\n");
			else
				fprintf(stdout, "I'm the father (PID=%d)\n", getpid());
			write(p_to_c[1], (n > 20) ? "Q" : "X", sizeof(char));
			if (n > 20)
				break;
			read(c_to_p[0], (void*)(&sig), sizeof(char));
		}
	} else {
		close(p_to_c[1]);
		close(c_to_p[0]);
		
		while (1) {
			read(p_to_c[0], (void*)(&sig), sizeof(char));

			if (sig == 'Q') {
				fprintf(stdout, "Child quitting\n");
				exit(EXIT_SUCCESS);
			}
			fprintf(stdout, "I'm the child (PID=%d)\n", getpid());
			write(c_to_p[1], "X", sizeof(char));
		}
	}

	return 0;
}
