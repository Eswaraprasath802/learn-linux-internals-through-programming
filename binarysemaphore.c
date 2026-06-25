#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *worker(void *arg)
{
    sem_wait(&sem);

    printf("Thread %ld entered\n", (long)arg);

    sleep(2);

    printf("Thread %ld leaving\n", (long)arg);

    sem_post(&sem);

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    sem_init(&sem, 0, 1);

    pthread_create(&t1, NULL, worker, (void *)1);
    pthread_create(&t2, NULL, worker, (void *)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}