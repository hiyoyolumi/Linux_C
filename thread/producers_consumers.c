#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

sem_t apple, orange, empty;
int count;

void *mom(void *arg);
void *pap(void *arg);
void *dau(void *arg);
void *son(void *arg);

int main()
{
    pthread_t thrd_mom, thrd_pap, thrd_dau, thrd_son;

    //初始化
    sem_init(&apple, 0, 0);
    sem_init(&orange, 0, 0);
    sem_init(&empty, 0, 1);

    //创建线程
    if(pthread_create(&thrd_mom, NULL, (void *)mom, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }
    if(pthread_create(&thrd_pap, NULL, (void *)pap, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }
    if(pthread_create(&thrd_dau, NULL, (void *)dau, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }
    if(pthread_create(&thrd_son, NULL, (void *)son, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }

    //等待线程结束
    if(pthread_join(thrd_mom, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }
    if(pthread_join(thrd_pap, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }
    if(pthread_join(thrd_dau, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }
    if(pthread_join(thrd_son, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }

    sem_destroy(&apple);
    sem_destroy(&orange);
    sem_destroy(&empty);
    return 0;
}

void *mom(void *arg)
{
    while(1)
    {
        sem_wait(&empty);
        printf("mom put an orange.\n");
        printf("mom --> %d", count);
        count++;
        sem_post(&orange);
        sleep(random() % 2);
    }
}

void *pap(void *arg)
{
    while(1)
    {
        sem_wait(&empty);
        printf("pap put an apple.\n");
        printf("pap --> %d", count);
        count++;
        sem_post(&apple);
        sleep(random() % 2);
    }
}

void *dau(void *arg)
{
    while(1)
    {
        sem_wait(&apple);
        printf("daughter get an apple.\n");
        printf("daughter <-- %d", count);
        count--;
        sem_post(&empty);
        sleep(random() % 2);
    }
}

void *son(void *arg)
{
    while(1)
    {
        sem_wait(&orange);
        printf("son get an orange.\n");
        printf("son <-- %d", count);
        count--;
        sem_post(&empty);
        sleep(random() % 2);
    }
}