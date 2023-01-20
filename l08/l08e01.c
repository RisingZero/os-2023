#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 30

typedef struct my_semaphore_s {
    int counter;
    int max;
    int nextTicketNumber;
    int turnNumber;
} *my_semaphore_t;

/* 
    Given implementation of an atomic test-and-set with increment
 */
int atomicIncrement (int *var) {
  int tmp = *var;
  *var = tmp + 1;
  return (tmp);
}

/* 
    Semaphore initialization
 */
my_semaphore_t init(int init_value) {
    if (init_value < 0)
        return NULL;

    my_semaphore_t sem = (my_semaphore_t)malloc(sizeof(struct my_semaphore_s));
    sem->max = init_value;
    sem->counter =  0;
    sem->nextTicketNumber = sem->turnNumber = 1;

    return sem;
}

/* 
    Request lock on semaphore
        if resource is available it is locked and function returns
        if resource is already locked caller is enqueued and the function blocks until available
 */
int lock(my_semaphore_t sem) {
    int ticketNumber;
    if (sem == NULL)
        return -1;
    ticketNumber = atomicIncrement(&(sem->nextTicketNumber));
    if (atomicIncrement(&(sem->counter)) >= sem->max) {
        // resource locked, enqueue and wait
        while(ticketNumber != sem->turnNumber);
    }

    return 0;
}

/* 
    Unlock a previously locked resource
 */
int unlock(my_semaphore_t sem) {
    if (sem == NULL)
        return -1;
    sem->counter--;
    atomicIncrement(&(sem->turnNumber));
    fprintf(stdout, "UNLOCK SEMAPHORE DEBUG - max: %d, counter: %d, turnNumber: %d, nextTicketNumber: %d\n", sem->max, sem->counter, sem->turnNumber, sem->nextTicketNumber);
    return 0;
}

typedef struct thread_data_s {
    my_semaphore_t sem;
    int id;
} thread_data_t;


void* thread_function(void *args) {
    thread_data_t* data = (thread_data_t*)args;

    fprintf(stdout, "Thread %d - Starting.\n", data->id);
    fprintf(stdout, "Thread %d - Requesting resource...\n", data->id);

    lock(data->sem);
    fprintf(stdout, "Thread %d - Entered critical section. Sleeping 5 seconds.\n", data->id);
    sleep(3);
    unlock(data->sem);
    fprintf(stdout, "Thread %d - Unlocked resource, quitting.\n", data->id);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    my_semaphore_t sem = init(4);
    thread_data_t *data;

    data = (thread_data_t*)malloc(NUM_THREADS * sizeof(thread_data_t));
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].id = i;
        data[i].sem = sem;
        pthread_create(&(threads[i]), NULL, thread_function, &(data[i]));
        if (i*2 == NUM_THREADS)
            sleep(6);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fprintf(stdout, "Threads done, closing main.\n");

    return 0;
}