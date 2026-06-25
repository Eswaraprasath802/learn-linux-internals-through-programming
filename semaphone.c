#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int counter = 0;
sem_t sem;

void *worker(void *arg)
{
    for(int i=0;i<100000;i++)
    {
        sem_wait(&sem);

        counter++;

        sem_post(&sem);
    }

    return NULL;
}

int main()
{
    pthread_t t1,t2;

    sem_init(&sem,0,1);

    pthread_create(&t1,NULL,worker,NULL);
    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    printf("counter=%d\n",counter);

    sem_destroy(&sem);

    return 0;
}