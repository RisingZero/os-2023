#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define OFFSET 'A'

enum TASKS { A, B, C, D, E, F, G, H, I, TASKS_NUM };

typedef struct task_data_s {
    int task_id;
    sem_t** task_dependancies;
    int N_dependancies;
    sem_t* notify_end;
    int notify_rounds;
} *task_data_t;

void* thread_task(void* args);

int main(int argc, char *argv[]) {
    pthread_t threads[TASKS_NUM];
    sem_t semaphores[TASKS_NUM];
    task_data_t task_data[TASKS_NUM];
    int i;
    enum TASKS task;

    for (i = 0; i < TASKS_NUM; i++) {
        sem_init(&semaphores[i], 0, 0);
        task_data[i] = (task_data_t)malloc(sizeof(struct task_data_s));
        if (task_data[i] == NULL) {
            fprintf(stderr, "ERROR allocating task data\n");
            exit(EXIT_FAILURE);
        }
        task_data[i]->task_id = i;
        task_data[i]->N_dependancies = 0;
        task_data[i]->notify_rounds = 0;
    }
    for (task = A; task < TASKS_NUM; task++) {
        task_data[task]->notify_end = &semaphores[task];
        switch (task) {
            case A:
                task_data[task]->notify_rounds = 3;
                break;
            case B:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[A];
                task_data[task]->notify_rounds = 1;
                break;
            case C:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[A];
                task_data[task]->notify_rounds = 2;
                break;
            case D:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[A];
                task_data[task]->notify_rounds = 1;
                break;
            case E:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[C];
                task_data[task]->notify_rounds = 1;
                break;
            case F:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[C];
                task_data[task]->notify_rounds = 1;
                break;
            case G:
                task_data[task]->N_dependancies = 2;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[E];
                task_data[task]->task_dependancies[1] = &semaphores[F];
                task_data[task]->notify_rounds = 1;
                break;
            case H:
                task_data[task]->N_dependancies = 1;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[D];
                task_data[task]->notify_rounds = 1;
                break;
            case I:
                task_data[task]->N_dependancies = 3;
                task_data[task]->task_dependancies = (sem_t**)malloc(task_data[task]->N_dependancies * sizeof(sem_t*));
                if (task_data[task]->task_dependancies == NULL) {
                    fprintf(stderr, "ERROR allocating task dependancies for task %d\n", task);
                    exit(EXIT_FAILURE);
                }
                task_data[task]->task_dependancies[0] = &semaphores[B];
                task_data[task]->task_dependancies[1] = &semaphores[G];
                task_data[task]->task_dependancies[2] = &semaphores[H];
                task_data[task]->notify_rounds = 0;
                break;
            default:
                break;
        }
    }
    
    while (1) {
        fprintf(stdout, "Cycle starting...\n");
        for (task = 0; task < TASKS_NUM; task++) {
            pthread_create(&threads[task], NULL, thread_task, &task_data[task]);
        }
        for (task = 0; task < TASKS_NUM; task++) {
            pthread_join(threads[task], NULL);
        }
        fprintf(stdout, "Cycle ended. Restarting...\n");
        sleep(3);
    }
    return 0;
}

void* thread_task(void* args) {
    task_data_t data = *(task_data_t*)args;
    int i;

    for (i = 0; i < data->N_dependancies; i++) {
        sem_wait(data->task_dependancies[i]);
    }
    fprintf(stdout, "Thread executing task %c\n", OFFSET + data->task_id);
    for (i = 0; i < data->notify_rounds; i++) {
        sem_post(data->notify_end);
    }

    return NULL;
}
