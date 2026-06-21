// sender.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
    char message[100];

    // Create FIFO (only the first time)
    mkfifo("mypipe", 0666);

    // Open FIFO for writing
    int fd = open("mypipe", O_WRONLY);

    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);

    write(fd, message, strlen(message) + 1);

    close(fd);

    return 0;
}