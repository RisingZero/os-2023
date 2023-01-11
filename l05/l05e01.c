#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define BUF_SIZE 512

int main(int argc, char *argv[]) {

	int buf[2], n, i;
	pid_t pid;
	char message[BUF_SIZE];

	pipe(buf);

	pid = fork();
       	if (pid > 0) {
		close(buf[0]);
		while (1) {
			fprintf(stdout, "> ");
			fscanf(stdin, "%s", message);
			
			n = strlen(message);
			write(buf[1], (void*)(&n), sizeof(int));
			write(buf[1], message, n);
			if (strcmp(message, "end") == 0) {
				fprintf(stdout, "Producer terminating\n");
				break;
			}
		}
	} else {
		close(buf[1]);
		while(1) {
			read(buf[0], (void*)(&n), sizeof(int));
			read(buf[0], message, n);
			if (n < BUF_SIZE)
				message[n] = '\0';
			
			if (strcmp(message, "end") == 0) {
				fprintf(stdout, "Consumer terminating\n");
				break;
			}

			for (i = 0; i < n; i++) {
				message[i] = toupper(message[i]); 
			}

			fprintf(stdout, "CHILD PID: %d - Received: %s\n", getpid(), message);
		}
	}

	return 0;
}
