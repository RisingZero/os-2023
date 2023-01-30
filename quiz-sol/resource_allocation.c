#include <unistd.h>
#include <semaphore.h>

#define MAX_RESOURCES 10

int resources[MAX_RESOURCES] = {0};
sem_t res, mut;

void init_resources() {
	sem_init(&res, 0, MAX_RESOURCES);
	sem_init(&mut, 0, 1);
}

int request(void) {
	int i;
	sem_wait(&res);
	sem_wait(&mut);
	for (i = 0; i < MAX_RESOURCES; i++) {
		if (resources[i] == 0) {
			resources[i] = 1;
			break;
		}
	}
	sem_post(&mut);
	return i;
}

void release(int i) {
	sem_wait(&mut);
	resources[i] = 0;
	sem_post(&mut);
	sem_post(&res);
}

