#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define THREAD_NUM 3
#define BUFFSIZE 100

void* read_task(void* args);
void* transform_task(void* args);
void* write_task(void* args);

typedef struct thread_data_s {
    char *infile;
    char *outfile;
    char buffer[BUFFSIZE+1];
    sem_t new_data; // tranform_task waits for new data from read_task
    sem_t writable_data; // write_task waits for buffer content to be writable after transformation
    sem_t data_written; // read_task waits for shared buffer to be written to file before pushing new data
    bool end_of_file;
} thread_data_t;

int main(int argc, char *argv[]) {

    pthread_t threads[THREAD_NUM];
    thread_data_t data;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src> <dst>", argv[0]);
        exit(2);
    }

    // Prepare thread data
    data.infile = (char*)malloc(strlen(argv[1]));
    data.outfile = (char*)malloc(strlen(argv[2]));
    data.end_of_file = false;
    strncpy(data.infile, argv[1], strlen(argv[1]));
    strncpy(data.outfile, argv[2], strlen(argv[2]));
    sem_init(&data.new_data, 0, 0);
    sem_init(&data.writable_data, 0, 0);
    sem_init(&data.data_written, 0, 1);

    // Create and start threads
    pthread_create(&threads[0], NULL, read_task, &data);
    pthread_create(&threads[1], NULL, transform_task, &data);
    pthread_create(&threads[2], NULL, write_task, &data);

    // Wait for threads to terminate
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void* read_task(void* args) {
    thread_data_t *data = (thread_data_t*)args;
    FILE *fp;
    char _internal_buffer[BUFFSIZE+1];

    fp = fopen(data->infile, "r");    
    if (fp == NULL) {
        fprintf(stderr, "ERROR opening input file\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(_internal_buffer, BUFFSIZE, fp) != NULL) {
        sem_wait(&(data->data_written));
        fprintf(stdout, "READ thread - Putting new line in shared buffer\n");
        _internal_buffer[strlen(_internal_buffer)-1] = '\0'; // Cutting \n
        strncpy(data->buffer, _internal_buffer, strlen(_internal_buffer)+1);
        sem_post(&(data->new_data));
    }

    fprintf(stdout, "READ thread - EOF detected, quitting\n");
    data->end_of_file = true;
    sem_post(&(data->new_data));
    pthread_exit(NULL);
}

void* transform_task(void* args) {
    thread_data_t *data = (thread_data_t*)args;
    int i, j;
    char _internal_buffer[BUFFSIZE+1] = "";

    while(1) {
        sem_wait(&(data->new_data));
        // Trasform line
        if (data->end_of_file)
            break;
        fprintf(stdout, "TRANSFORM thread - Transforming line in shared buffer\n");
        j = 0;
        for (i = strlen(data->buffer) - 1; i >= 0; i--) {
            _internal_buffer[j++] = toupper(data->buffer[i]);
        }
        strncpy(data->buffer, _internal_buffer, strlen(_internal_buffer)+1);
        sem_post(&(data->writable_data));
    }
    fprintf(stdout, "TRANSFORM thread - EOF detected, quitting\n");
    sem_post(&(data->writable_data));
    pthread_exit(NULL);
}

void* write_task(void* args) {
    thread_data_t *data = (thread_data_t*)args;
    FILE *fp;
    char _internal_buffer[BUFFSIZE+1] = "";

    fp = fopen(data->outfile, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR opening output file\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        sem_wait(&(data->writable_data));
        if (data->end_of_file)
            break;
        // Copy shared buffer locally and unlock other threads
        fprintf(stdout, "WRITE thread - Writing shared buffer to file\n");
        strncpy(_internal_buffer, data->buffer, strlen(data->buffer)+1);
        sem_post(&(data->data_written));

        // Write to file from internal buffer
        fprintf(fp, "%s\n", _internal_buffer);
    }
    fprintf(stdout, "WRITE thread - EOF detected, quitting\n");
    pthread_exit(NULL);
}
