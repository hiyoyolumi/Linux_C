#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

pthread_once_t once  = PTHREAD_ONCE_INIT;

void run(void);
void *thread1(void *arg);
void *thread2(void *arg);

int main()
{
    pthread_t thid1, thid2;

    pthread_create(&thid1, NULL, thread1, NULL);
    pthread_create(&thid2, NULL, thread2, NULL);
    sleep(3);
    printf("main thread exit! \n");
    exit(0);
}

void run(void)
{
    printf("Fuction run is running in thread %u\n", pthread_self());
}

void *thread1(void *arg)
{
    pthread_t thid = pthread_self();    //返回线程自身ID
    printf("Current thread's ID is %u\n", thid);
    pthread_once(&once, run);           //保证run函数在进程中只执行一次
    printf("thread1 ends\n");
}

void *thread2(void *arg)
{
    pthread_t thid = pthread_self();
    printf("Current thread's ID is %u\n", thid);
    pthread_once(&once, run);
    printf("thread2 ends\n");
}