#include <unistd.h>
#include <semaphore.h>

sem_t a, b;

sem_init(&a, 0, 0);
sem_init(&b, 0, 0);

void funz_a() {
	sem_post(&b);
	sem_wait(&a);
}

void funz_b() {
	sem_post(&a);
	sem_wait(&b);
}
