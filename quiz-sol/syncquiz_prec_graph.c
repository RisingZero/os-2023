#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

sem_t s1, s2, s3;

void* P1(void* args) {
	while(1) {
		sem_wait(&s1);
		printf("P1\n");
		sem_post(&s2);
		sem_post(&s2);
	}
}

void* P2(void* args) {
	while(1) {
		sem_wait(&s2);
		printf("P2\n");
		sem_post(&s3);
	}
}

void* P3(void* args) {
	while(1) {
		sem_wait(&s3);
		sem_wait(&s3);
		printf("P3\n");
		sem_post(&s1);
	}
}

int main(int argc, char *argv[]) {

	pthread_t p1, p21, p22, p3;
	
	sem_init(&s1, 0, 1);
	sem_init(&s2, 0, 0);
	sem_init(&s3, 0, 0);

	pthread_create(&p1, NULL, P1, NULL);
	pthread_create(&p21, NULL, P2, NULL);
	pthread_create(&p22, NULL, P2, NULL);
	pthread_create(&p3, NULL, P3, NULL);

	pthread_join(p1, NULL);
	return 0;
}

