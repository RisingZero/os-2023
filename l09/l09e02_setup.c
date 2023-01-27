#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int fd;
    int N = atoi(argv[1]);
    int n;
    float c, x;

    fd = creat("test2", S_IRWXU | S_IRWXG | S_IRWXO);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 3; j++) {
            n = rand() % 4;
            c = (rand() % 50) / (rand() % 7 +1);
            x = (rand() % 23) / (rand() % 8 +1);
            write(fd, &c, sizeof(float));
            write(fd, &x, sizeof(float));
            write(fd, &n, sizeof(int));
        }   
    }
    close(fd);
    printf("done\n");
    return 0;
}