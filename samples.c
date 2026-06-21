#include <stdio.h>
#include <unistd.h>

int main() {

    printf("Before exec()\n");

    execl("/bin/ls", "ls", "-la", NULL);

    printf("This line never executes\n");

    return 0;
}