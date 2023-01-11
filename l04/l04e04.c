#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void sigHandler(int sig);
void producer(char* filename, pid_t consumer_pid);
void consumer(char* filename);

int main(int argc, char * argv[]) {
	pid_t pid,cid;
	char *filename = "tmp.txt";
	
	if (signal(SIGUSR1, sigHandler) == SIG_ERR) {
		fprintf(stderr, "Error setting handler\n");
		exit(EXIT_FAILURE);
	}
	
	cid = fork();
	if (cid == 0) {
		consumer(filename);
		fprintf(stdout, "Consumer terminating.\n");
		exit(EXIT_SUCCESS);
	}
	pid = fork();
	if (pid == 0) {
		producer(filename, cid);
		fprintf(stdout, "Producer terminating.\n");
		exit(EXIT_SUCCESS);
	}
	wait((int *)0);
	wait((int *)0);
	fprintf(stdout, "Father terminating.\n");
	return 0;
}

void sigHandler(int sig) {
	return;
}

void producer(char* filename, pid_t consumer_pid) {
	FILE *fp;
	char message[1024];

	while(1) {
		fp = fopen(filename, "w");
		if (fp == NULL) {
			fprintf(stderr, "(CHILD PRODUCER) PID: %d - ERROR opening file\n", getpid());
			exit(EXIT_FAILURE);
		}
		setvbuf(fp, NULL, _IONBF, 0);
	
		fprintf(stdout, "> ");
		fscanf(stdin, "%s", message);
		fprintf(fp, "%d %s\n", getpid(), message);
		sleep(1);
		kill(consumer_pid, SIGUSR1);
		if (strcmp(message, "end") == 0)
			break;
		pause();
	}
	return;
}

void consumer(char* filename) {
	FILE* fp;
	int num_messages = 0, producer_pid, i;
	char message[1024];

	while(1) {
		pause();
		fp = fopen(filename, "r");
		if (fp == NULL) {
			fprintf(stderr, "(CHILD CONSUMER) PID: %d - ERROR opening file\n", getpid());
			exit(EXIT_FAILURE);
		}
		setvbuf(fp, NULL, _IONBF, 0);
		
		fscanf(fp, "%d %s\n", &producer_pid, message);
		if (strcmp(message, "end") == 0)
			break;
		num_messages++;
		for (i = 0; i < strlen(message); i++) {
			message[i] = toupper(message[i]);
		}
		fprintf(stdout, "CONSUMER PID: %d - RECEIVED (%d): %s\n", getpid(), num_messages, message);

		sleep(1);
		kill(producer_pid, SIGUSR1);
	}
	return;
}
