#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    const size_t SIZE = 512 * 1024 * 1024;   // 512 MB

    char *p = malloc(SIZE);

    if (p == NULL) {
        perror("malloc");
        return 1;
    }

    printf("Memory allocated.\n");
    printf("Press ENTER to start touching pages...");
    getchar();

    // Touch one byte per page
    for (size_t i = 0; i < SIZE; i += 4096) {
        p[i] = 1;
    }

    printf("Finished touching all pages.\n");

    printf("Press ENTER to exit...");
    getchar();

    free(p);

    return 0;
}