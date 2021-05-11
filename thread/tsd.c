#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

pthread_key_t key;

void *thread1(void *arg);
void *thread2(void *arg);

int main()
{
    pthread_t thid1;
    printf("main thread begins running\n");
    pthread_key_create(&key, NULL);
    pthread_create(&thid1, NULL, (void *)thread1, NULL);
    sleep(3);
    pthread_key_delete(key);
    printf("main thread exit\n");
    return 0;
}

void *thread1(void *arg)
{
    long tsd = 0;
    pthread_t thid2;
    
    printf("thread %ld is running\n", pthread_self());
    pthread_setspecific(key, (void *)tsd);
    pthread_create(&thid2, NULL, (void *)thread2, NULL);
    sleep(2);
    printf("thread %ld returns %p\n", pthread_self(), pthread_getspecific(key));    
}

void *thread2(void *arg)
{
    long tsd = 5;
    printf("thread %ld is running\n", pthread_self());
    pthread_setspecific(key, (void *)tsd);
    printf("thread %ld returns %p\n", pthread_self(), pthread_getspecific(key));
}