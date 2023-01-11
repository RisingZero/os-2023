#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void childSwap(char *filename, int pos, int N);

int main(int argc, char *argv[]) {

	int fd, N = 20, i, j, childs_at_step;
	pid_t *childs;

	srand(time(NULL));

	if (argc != 2) {
		fprintf(stderr, "Indicate filename\n");
		exit(EXIT_FAILURE);
	}

	fd = creat(argv[1], S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1) {
		fprintf(stderr, "ERROR creating file\n");
		exit(EXIT_FAILURE);
	}
	write(fd, (void*)(&N), sizeof(int));
	fprintf(stdout, "Starting with array:\n[");
	for (i = 0; i < N; i++) {
		j = rand() % 100;
		fprintf(stdout, "%d", j);
		if (i != N-1)
			fprintf(stdout, ",");
		write(fd, (void*)(&j), sizeof(int));
	}
	fprintf(stdout, "]\n");
	close(fd);

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "ERROR opening file\n");
		exit(EXIT_FAILURE);
	}

	if (read(fd, (void *)(&N), sizeof(int)) != sizeof(int)) {
		fprintf(stderr, "Failed reading first number N\n");
		exit(EXIT_FAILURE);
	}
	close(fd);

	childs = (pid_t*)malloc(N * sizeof(pid_t));
	if (childs == NULL) {
		fprintf(stderr, "Error allocating vector of childs\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < N; i++) {
		childs_at_step = 0;
		for (j = 0; j < N; j += 2) {
			childs[j] = fork();
			if (childs[j] == 0) {
				// call for child to swap on even position 2k, 2k+1
				childSwap(argv[1], j, N);
			} else
				childs_at_step++;
		}
		while (childs_at_step != 0) {
			wait((int *)0);
			childs_at_step--;
		}
		for (j = 1; j < N; j += 2) {
			childs[j] = fork();
		       	if (childs[j] == 0) {
		       		// call for child to swap on odd position 2k+1, 2k+2
				childSwap(argv[1], j, N);
		       	} else
				childs_at_step++;
		}
		while (childs_at_step != 0) {
			wait((int*)0);
			childs_at_step--;
		}
	}

	fprintf(stdout, "Array reordered:\n");

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "ERROR reopening file\n");
		exit(EXIT_FAILURE);
	}
	read(fd, (void*)(&N), sizeof(int)); // Read first number N

	fprintf(stdout, "[");
	for (i = 0; i < N; i++) {
		if (read(fd, (void*)(&j), sizeof(int)) != sizeof(int)) {
			fprintf(stderr, "ERROR number of elements not coherent\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "%d", j);
		if (i != N-1)
			fprintf(stdout, ",");
	}
	fprintf(stdout, "]\n");
	close(fd);

	free(childs);

	return 0;
}

void childSwap(char *filename, int pos, int N) {
	int fd, a, b;

	if (pos == N-1)
		exit(EXIT_SUCCESS);

	fd = open(filename, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "CHILD (%d): ERROR opening file\n", getpid());
		exit(EXIT_FAILURE);
	}	
	pos++;
	// Read two numbers
	lseek(fd, pos*sizeof(int), SEEK_SET);
	read(fd, (void*)(&a), sizeof(int));
	read(fd, (void*)(&b), sizeof(int));

	if (a > b) {
		lseek(fd, pos*sizeof(int), SEEK_SET);
		write(fd, (void*)(&b), sizeof(int));
		write(fd, (void*)(&a), sizeof(int));
	}
	close(fd);
	exit(EXIT_SUCCESS);
}
