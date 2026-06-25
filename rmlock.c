#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int balance = 1000;

pthread_rwlock_t rwlock;

void *reader(void *arg)
{
    int id = *(int *)arg;

    for(int i=0;i<3;i++)
    {
        pthread_rwlock_rdlock(&rwlock);

        printf("Reader %d reads balance=%d\n",
               id, balance);

        sleep(1);

        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }

    return NULL;
}

void *writer(void *arg)
{
    for(int i=0;i<3;i++)
    {
        pthread_rwlock_wrlock(&rwlock);

        balance += 100;

        printf("Writer updated balance=%d\n",
               balance);

        pthread_rwlock_unlock(&rwlock);

        sleep(2);
    }

    return NULL;
}

int main()
{
    pthread_t r1,r2,w;

    int id1=1;
    int id2=2;

    pthread_rwlock_init(&rwlock,NULL);

    pthread_create(&r1,NULL,reader,&id1);
    pthread_create(&r2,NULL,reader,&id2);
    pthread_create(&w,NULL,writer,NULL);

    pthread_join(r1,NULL);
    pthread_join(r2,NULL);
    pthread_join(w,NULL);

    return 0;
}