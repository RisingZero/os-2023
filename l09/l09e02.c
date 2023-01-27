#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define POLYNOMIAL_SIZE 3

typedef struct thread_data_s {
    int tid;
    float result;
    char *filename;
    int *n_producers;
    sem_t *mutex;
    sem_t *read_monomials;
    sem_t *monomials_consume;
    bool *end_of_file;
} thread_data_t;

void* compute_monomial(void* args);
void* compute_poly(void* args);

int main(int argc, char *argv[]) {

    thread_data_t data[POLYNOMIAL_SIZE+1];
    pthread_t threads[POLYNOMIAL_SIZE+1];
    sem_t mutex, read_monomials, monomials_consume;
    int i, n_producers = 0;
    bool end_of_file = false;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(2);
    }

    sem_init(&mutex, 0, 1);
    sem_init(&read_monomials, 0, 0);
    sem_init(&monomials_consume, 0, 3);
    
    for (i = 0; i < POLYNOMIAL_SIZE + 1; i++) {
        data[i].tid = i;
        data[i].filename = argv[1];
        data[i].result = 0;
        data[i].n_producers = &n_producers;
        data[i].mutex = &mutex;
        data[i].read_monomials = &read_monomials;
        data[i].monomials_consume = &monomials_consume;
        data[i].end_of_file = &end_of_file;
    }

    for (i = 0; i < POLYNOMIAL_SIZE; i++) {
        pthread_create(&threads[i], NULL, compute_monomial, &data[i]);
    }
    pthread_create(&threads[POLYNOMIAL_SIZE], NULL, compute_poly, &data);

    for (i = 0; i < POLYNOMIAL_SIZE + 1; i++) {
        pthread_join(threads[i], NULL);
    }
    fprintf(stdout, "MAIN thread - quitting\n");

    return 0;
}

void* compute_monomial(void* args) {
    thread_data_t* data = (thread_data_t*)args;
    int fd;
    int n; float c, x;
    int i;
    int status = 0;

    fd = open(data->filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "#%d thread - error opening input file\n", data->tid);
        exit(EXIT_FAILURE);
    }
    lseek(fd, data->tid * (sizeof(float) * 2 + sizeof(int)), SEEK_SET);

    while(1) {
        sem_wait(data->monomials_consume);
        sem_wait(data->mutex);
        *data->n_producers += 1;
        sem_post(data->mutex);
        
        // read one monomial in <tid> position and compute value
        status = read(fd, &c, sizeof(float));
        status = read(fd, &x, sizeof(float));
        status = read(fd, &n, sizeof(int));
        if (status == -1 || status == 0) {
            *data->end_of_file = true;
        }
        data->result = c;
        for (i = 0; i < n; i++) {
            data->result *= x;
        }
        status = lseek(fd, sizeof(float)*4 + sizeof(int)*2, SEEK_CUR);
        if (status == -1) {
            *data->end_of_file = true;
        }

        sem_wait(data->mutex);
        *data->n_producers -= 1;
        if (*data->n_producers == 0)
            sem_post(data->read_monomials);
        sem_post(data->mutex);

        if (*data->end_of_file)
            break;
    }
    pthread_exit(EXIT_SUCCESS);
}

void* compute_poly(void* args) {
    thread_data_t* data;
    data = (thread_data_t*)args;
    int i;
    float result;

    while(1) {
        sem_wait((data + POLYNOMIAL_SIZE)->read_monomials);

        // consume read monomials and compute poly sum
        if (*((data + POLYNOMIAL_SIZE)->end_of_file))
            break;
        result = 0;
        for (i = 0; i < POLYNOMIAL_SIZE; i++) {
            result += (data + i)->result;
        }
        fprintf(stdout, "Polynomial value: %.2f\n", result);

        for (i = 0; i < POLYNOMIAL_SIZE; i++) {
            sem_post((data + POLYNOMIAL_SIZE)->monomials_consume);
        }
    }

    pthread_exit(EXIT_SUCCESS);

}