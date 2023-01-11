#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void binary(int *, int);

int main(int argc, char *argv[]) {
  int n;
  int *vet;

  n = atoi (argv[1]);

  vet = (int *)malloc(n * sizeof(int));
  if (vet == NULL) {
    printf("Allocatin Error.\n");
    exit(1);
  }

  printf("Binary Numbers:\n");
  binary(vet, n);
  free(vet);

  return 0;
}

void binary (int *vet, int n) {
	int i;

	for  (i = 0; i < n; i++) {
		if (fork() > 0) {
			vet[i] = 0;
		} else {
			vet[i] = 1;
		}
	}
	
	fprintf(stdout, "PID %d - binary number: ", getpid());
	for (i = 0; i < n; i++) {
		fprintf(stdout, "%d", vet[i]);
	}
	fprintf(stdout, "\n");
}
