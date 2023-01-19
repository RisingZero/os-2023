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
    int *vet;
    int N;
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
	FILE *fi;
	int i;

	fi = fopen(data->infile, "r");
	if (fi == NULL) {
		fprintf(stderr, "Thread #%d - ERROR opening files\n", data->thread_num);
		data->exit_status = EXIT_FAILURE;
		pthread_exit(NULL);
	}
	
	// Read N on first line and allocate vector
	fscanf(fi, "%d", &data->N);
	data->vet = (int*)malloc(data->N * sizeof(int));
	if (data->vet == NULL) {
		fprintf(stderr, "Thread #%d - ERROR allocating\n", data->thread_num);
		data->exit_status = EXIT_FAILURE;
		pthread_exit(NULL);
	}
	
	// Read N values
	for (i = 0; i < data->N; i++) {
		fscanf(fi, "%d", &(data->vet)[i]);
	}

	// Sort the vector of integers
	qsort(data->vet, data->N, sizeof(int), compare_int);

	fclose(fi);
	return NULL;
}

int merge_results(int **dst_vet, int dst_N, int *src_vet, int src_N);

int main(int argc, char *argv[]) {
	
    FILE *fo;
	int n, i, status;
    int *merged = NULL, merged_N = 0;
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
        else
            merged_N = merge_results(&merged, merged_N, data[i].vet, data[i].N);
	}
	
	if (status == EXIT_SUCCESS)
		fprintf(stdout, "Jobs completed!\n");
	else if (status == EXIT_FAILURE)
		fprintf(stderr, "Threads reported something went wrong...\n");

    fo = fopen(argv[3], "w");
    if (fo == NULL) {
		fprintf(stderr, "Main thread - ERROR opening output file\n");
        exit(EXIT_FAILURE);
	}

    // Write on output file the sorted vector
	fprintf(fo, "%d\n", merged_N);
	for (i = 0; i < merged_N; i++) {
		fprintf(fo, "%d\n", merged[i]);
	}
	fclose(fo);
    free(merged);

	for (i = 0; i < n; i++) {
		free(data[i].infile);
		free(data[i].outfile);
        if (data[i].vet != NULL)
            free(data[i].vet);
	}
	free(data);
	free(threads);
	return 0;
}

int merge_results(int **dst_vet, int dst_N, int *src_vet, int src_N) {
    int* v1 = *dst_vet;
    int* v2 = src_vet;
    int i = 0, j = 0, pos = 0, N = dst_N + src_N;

    *dst_vet = (int*)malloc(N * sizeof(int));

    if (v1 != NULL) {
        while (pos != N && i < dst_N && j < src_N) {
            if (v1[i] <= v2[j]) {
                (*dst_vet)[pos++] = v1[i++];
            } else {
                (*dst_vet)[pos++] = v2[j++];
            }
        }
        if (i < dst_N) {
            for (; i < dst_N; i++) {
                (*dst_vet)[pos++] = v1[i];
            }
        }
        if (j < src_N) {
            for (; j < src_N; j++) {
                (*dst_vet)[pos++] = v2[j];
            }
        }
        free(v1);
    } else {
        for (i = 0; i < N; i++) {
            (*dst_vet)[i] = src_vet[i];
        }
    }
    
    return N;
}