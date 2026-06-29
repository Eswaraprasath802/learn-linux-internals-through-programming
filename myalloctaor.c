#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

void *my_malloc(size_t size)
{
    void *ptr = mmap(
        NULL,                       // Kernel chooses address
        size,                       // Number of bytes
        PROT_READ | PROT_WRITE,     // Read and write
        MAP_PRIVATE | MAP_ANONYMOUS,// Private anonymous mapping
        -1,
        0);

    if (ptr == MAP_FAILED)
        return NULL;

    return ptr;
}

void my_free(void *ptr, size_t size)
{
    munmap(ptr, size);
}

int main()
{
    size_t size = 4096;

    char *buffer = my_malloc(size);

    if (!buffer)
    {
        printf("Allocation failed\n");
        return 1;
    }

    strcpy(buffer, "Hello from mmap allocator!");

    printf("%s\n", buffer);

    my_free(buffer, size);

    return 0;
}