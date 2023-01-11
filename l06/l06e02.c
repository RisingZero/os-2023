/*
 * 	gcc -Wall -g l06e02.c -lpthread -o main
 * 	./main 2 strA strB
 */

#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define TXT_EXTENSION ".txt"

struct thread_data {
	char *infile;
	char *outfile;
	int thread_num;
	int exit_status;
};

int compare_int(const void* a, const void* b) {
	if (*(int*)a>*(int*)b)
		return 1;
	else if (*(int*)a < *(int*)b)
		return -1;
	else
		return 0;
}

void* sort_file(void *args) {
	struct thread_data *data = (struct thread_data*)args;
	FILE *fi, *fo;
	int N, i, *vet;

	fi = fopen(data->infile, "r");
	fo = fopen(data->outfile, "w");
	if (fi == NULL || fo == NULL) {
		fprintf(stderr, "Thread #%d - ERROR opening files\n", data->thread_num);
		data->exit_status = EXIT_FAILURE;
		pthread_exit(NULL);
	}
	
	// Read N on first line and allocate vector
	fscanf(fi, "%d", &N);
	vet = (int*)malloc(N * sizeof(int));
	if (vet == NULL) {
		fprintf(stderr, "Thread #%d - ERROR allocating\n", data->thread_num);
		data->exit_status = EXIT_FAILURE;
		pthread_exit(NULL);
	}
	
	// Read N values
	for (i = 0; i < N; i++) {
		fscanf(fi, "%d", &vet[i]);
	}

	// Sort the vector of integers
	qsort(vet, N, sizeof(int), compare_int);

	// Write on output file the sorted vector
	fprintf(fo, "%d\n", N);
	for (i = 0; i < N; i++) {
		fprintf(fo, "%d\n", vet[i]);
	}

	fclose(fi);
	fclose(fo);
	free(vet);	
	return NULL;
}

int main(int argc, char *argv[]) {
	
	int n, i, status;
	pthread_t *threads;
	struct thread_data *data;

	
	if (argc != 4) {
		fprintf(stderr, "ERROR: indicate 3 parameters:  n strA strB\n");
		exit(EXIT_FAILURE);
	}

	n = atoi(argv[1]);

	threads = (pthread_t*)malloc(n*sizeof(pthread_t));
	data = (struct thread_data*)malloc(n*sizeof(struct thread_data));
	if (threads == NULL || data == NULL) {
		fprintf(stderr, "ERROR allocating vector of threads\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++) {
		data[i].infile = (char*)malloc((strlen(argv[2])+strlen(TXT_EXTENSION)+strlen(argv[1])+1) * sizeof(char));
		data[i].outfile = (char*)malloc((strlen(argv[3])+strlen(TXT_EXTENSION)+strlen(argv[1])+1) * sizeof(char));
		sprintf(data[i].infile, "%s%d%s", argv[2], i+1, TXT_EXTENSION);
		sprintf(data[i].outfile, "%s%d%s", argv[3], i+1, TXT_EXTENSION);
		data[i].thread_num = i;
		data[i].exit_status = 0;
		pthread_create(&threads[i], NULL, sort_file, &data[i]);
	}

	fprintf(stdout, "Waiting for jobs to finish...\n");
	
	status = EXIT_SUCCESS;
	for(i = 0; i < n; i++) {
		pthread_join(threads[i], NULL);
		if (data[i].exit_status == EXIT_FAILURE)
			status = EXIT_FAILURE;
	}
	
	if (status == EXIT_SUCCESS)
		fprintf(stdout, "Jobs completed!\n");
	else if (status == EXIT_FAILURE)
		fprintf(stderr, "Threads reported something went wrong...\n");

	for (i = 0; i < n; i++) {
		free(data[i].infile);
		free(data[i].outfile);
	}
	free(data);
	free(threads);
	return 0;
}
