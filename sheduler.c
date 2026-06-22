#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n = 3;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            printf("Child %d started. PID = %d\n", i + 1, getpid());

            while (1) {
                // Consume CPU
            }

            exit(0);
        }
    }

    sleep(2);

    printf("Parent PID = %d\n", getpid());

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}