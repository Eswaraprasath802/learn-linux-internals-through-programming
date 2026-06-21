#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    if(fork()==0){
        printf("Child Process\n");
    }
    else{
        wait(NULL); // parent wait until child complete
        printf("Parent Process\n");
    }

    return 0;
}