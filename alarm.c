#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)
{
    printf("Alarm Triggered!\n");
}

int main()
{
    signal(SIGALRM, handler);

    alarm(3);

    pause();

    return 0;
}