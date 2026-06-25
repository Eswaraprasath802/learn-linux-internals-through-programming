#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t empty;
sem_t full;

int buffer_count = 0;

void *producer(void *arg)
{
    for(int i=1;i<=5;i++)
    {
        sem_wait(&empty);

        buffer_count++;

        printf("Produced item %d, buffer=%d\n",
               i, buffer_count);

        sem_post(&full);

        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg)
{
    for(int i=1;i<=5;i++)
    {
        sem_wait(&full);

        buffer_count--;

        printf("Consumed item %d, buffer=%d\n",
               i, buffer_count);

        sem_post(&empty);

        sleep(2);
    }

    return NULL;
}

int main()
{
    pthread_t p,c;

    sem_init(&empty,0,3);
    sem_init(&full,0,0);

    pthread_create(&p,NULL,producer,NULL);
    pthread_create(&c,NULL,consumer,NULL);

    pthread_join(p,NULL);
    pthread_join(c,NULL);

    return 0;
}