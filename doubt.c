#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
 
sem_t hai;
int buffer=0;
void *welcome(void *h){
  for (int i=0;i<4;i++){
    sem_wait(&hai);
    buffer++;
    printf("%d",buffer);
    sem_post(&hai);
  }
}
void main(){
  pthread_t p,c;
  sem_init(&hai,0,3);
  pthread_create(&p,NULL,&welcome,NULL);
  pthread_join(p,NULL);
}