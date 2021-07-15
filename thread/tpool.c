#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

#include "tpool.h"

tpool_t *tpool = NULL;

//从任务链表中取出并执行func
void * thread_routine(void *arg)
{
    node *work;

    while(1)
    {
        pthread_mutex_lock(&tpool->mutex);

        while(!tpool->head && !tpool->shutdown) //如果队列为空就等待
        {
            pthread_cond_wait(&tpool->queue_ready, &tpool->mutex);
        }
        if(tpool->shutdown)
        {
            pthread_mutex_unlock(&tpool->mutex);
            pthread_exit(NULL);
        }
        
        work = tpool->head;
        tpool->head = tpool->head->next;

        pthread_mutex_unlock(&tpool->mutex);
        //printf("now is thread:%u\n", pthread_self());
        work->routine(work->arg);
        free(work); //
    }

    return NULL;
}

//销毁线程池
void tpool_destory()
{
    node *q;

    if(tpool->shutdown)
    {
        return;
    }
    tpool->shutdown = 1;

    pthread_mutex_lock(&tpool->mutex);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->mutex);

    for(int i=0; i<tpool->max_pthread_num; i++)
    {
        pthread_join(tpool->thr_id[i], NULL);
    }
    free(tpool->thr_id);

    while(tpool->head)  //不会进入循环
    {
        //printf("hello in %d\n", __LINE__);
        q = tpool->head;
        tpool->head = tpool->head->next;
        free(q);
    }

    free(tpool);

    pthread_mutex_destroy(&tpool->mutex);
    pthread_cond_destroy(&tpool->queue_ready);
}

//创建线程池
int tpool_create(int max_pthread_num)
{
    tpool = (tpool_t *)calloc(1, sizeof(tpool_t));
    if(!tpool)
    {
        printf("%s:calloc failed in %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    //初始化
    tpool->shutdown = 0;
    tpool->max_pthread_num = max_pthread_num;
    tpool->head = NULL;
    if(pthread_mutex_init(&tpool->mutex, NULL) != 0)
    {
        printf("%s:pthread_mutex_init failed\n", __FUNCTION__);
        exit(1);
    }
    if(pthread_cond_init(&tpool->queue_ready, NULL) != 0)
    {
        printf("%s:pthread_cond_init failed\n", __FUNCTION__);
        exit(1);
    }

    //创建工作者线程
    tpool->thr_id = (tpool_t *)calloc(max_pthread_num, sizeof(tpool_t));
    if(!tpool->thr_id)
    {
        printf("%s:calloc failed in %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    for(int i=0; i<max_pthread_num; i++)
    {
        if(pthread_create(&tpool->thr_id[i], NULL, thread_routine, NULL) != 0)
        {
            printf("%d:pthread_create failed\n", __LINE__);
            exit(1);
        }
        //printf("now is thread:%d\n");
    }

    return 0;
}

//向线程池添加任务
int tpool_add_work(void* (*routine)(void *arg), void *arg)
{
    node *member, *work;

    if(!routine)
    {
        printf("%s:Invalid argument\n", __FUNCTION__);
        return -1;
    }

    work = (node *)malloc(sizeof(node));
    if(!work)
    {
        printf("%s:malloc failed\n", __FUNCTION__);
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;

    pthread_mutex_lock(&tpool->mutex);

    member = tpool->head;
    if(!member)
    {
        tpool->head = work;
    }
    else
    {
        while(member->next)
        {
            member = member->next;
        }
        member->next = work;
    }

    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->mutex);

    return 0;
}