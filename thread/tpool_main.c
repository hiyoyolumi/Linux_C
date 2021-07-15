#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include "tpool.h"

void* func(void *arg);

int main()
{
    if(tpool_create(2) != 0)
    {
        printf("tpool_create failed\n");
        exit(1);
    }
    for(int i=0; i<10; i++)
    {
        tpool_add_work(func, (void *)i);
    }
    sleep(2);
    tpool_destory();

    return 0;
}

void* func(void *arg)
{
    printf("thread %d\n", (int)arg);
    
    return NULL;
}