// receiver.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
    char message[100];

    // Create FIFO (only the first time)
    mkfifo("mypipe", 0666);

    // Open FIFO for reading
    int fd = open("mypipe", O_RDONLY);

    read(fd, message, sizeof(message));

    printf("Received Message: %s", message);

    close(fd);

    return 0;
}