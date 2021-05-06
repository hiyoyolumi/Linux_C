#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

int * thread(void *arg);

int main()
{
    pthread_t thid;

    printf("main thread, ID is %u\n", pthread_self());
    if( pthread_create(&thid, NULL, (void *)thread, NULL) != 0)
    {
        printf("thread creation failed\n");
        exit(1);
    }
    sleep(1);
    exit(0);
}

int * thread(void *arg)
{
    pthread_t newthid;

    newthid = pthread_self();
    printf("this is a new thread, thread ID = %u\n", newthid);
    return NULL;
}