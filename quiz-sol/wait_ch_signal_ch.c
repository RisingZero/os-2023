#include <unistd.h>
#include <semaphore.h>

typedef struct sem_ch_s {
	int cnt;
	sem_t s;
	sem_t mutex;
} sem_ch_t;

sem_ch_t semaphores[10];

void init_ch() {
	int i;
	for (i = 0; i < 10; i++) {
		semaphores[i].cnt = 0;
		sem_init(&(semaphores[i].s), 0, 0);
		sem_init(&(semaphores[i].mutex), 0, 1);
	}
}

void wait_ch(int id) {
	sem_wait(&(semaphores[id].mutex));
	semaphores[id].cnt++;
	sem_post(&(semaphores[id].mutex));
	sem_wait(&(semaphores[id].s));
}

void signal_ch(int id) {
	int i;
	sem_wait(&(semaphores[id].mutex));
	for (i = 0; i < semaphores[id].cnt; i++) {
		sem_post(&(semaphores[id].s));
	}
	semaphores[id].cnt = 0;
	sem_post(&(semaphores[id].mutex));
}

