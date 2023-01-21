#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    pid_t pid1 = getpid(), pid2, pid3;
    char buf[64];
    int sem1[2], sem2[2], sem3[2];
  
    
    while (1) {
        pipe(sem1);
        pipe(sem2);
        printf("A\n");
        pid2 = fork();
        if (pid2 > 0) {
            pid3 = fork();
            if (pid3 > 0) {
                // Process 1
                close(sem1[1]);
                close(sem2[1]);
                printf("B\n");
                read(sem1[0], buf, 1);
                read(sem2[0], buf, 1);
                printf("I\n");
                printf("----------\n");
                sleep(5);
            } else {
                // Process 3
                close(sem2[0]);
                printf("D\n");
                printf("H\n");
                write(sem2[1], "X", 1);
                exit(EXIT_SUCCESS);
            }
        } else {
            // Process 2
            close(sem1[0]);
            printf("C\n");
            pipe(sem3);
            if (fork() > 0) {
                // Process 2.1
                close(sem3[1]);
                printf("E\n");
                read(sem3[0], buf, 1);
                printf("G\n");
                write(sem1[1], "X", 1);
                exit(EXIT_SUCCESS);
            } else {
                // Process 2.2
                close(sem3[0]);
                printf("F\n");
                write(sem3[1], "X", 1);
                exit(EXIT_SUCCESS);
            }
        }
        
    }

    return 0;
}