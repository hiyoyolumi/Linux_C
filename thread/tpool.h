#ifndef THREAD_POOL_H__
 #define THREAD_POOL_H__

#include<pthread.h>

typedef struct tpool_work
{
    void * (*routine)(void *arg);   //任务函数
    void * arg;                     //函数参数
    struct tpool_work *next;         //后继节点
}node;

typedef struct tpool
{
    int shutdown;   //线程池是否销毁
    int max_pthread_num;    //最大线程数
    pthread_t *thr_id;      //线程id数组
    node *head;             //线程链表
    pthread_mutex_t  mutex;
    pthread_cond_t queue_ready;
}tpool_t;

int tpool_add_work(void* (*routine)(void *arg), void *arg);

int tpool_create(int max_pthread_num);

void tpool_destory();

void * thread_routine(void *arg);

#endif