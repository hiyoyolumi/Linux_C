#include"threadpool.h"

struct threadpool* threadpool_init(int thread_num, int queue_max_num)
{
    struct threadpool *pool = NULL;
    do
    {
        pool = (struct threadpool *)malloc(sizeof(struct threadpool));
        if(pool == NULL)
        {
            printf("failed to malloc threadpool!\n");
            break;
        }
        pool->thread_num = thread_num;
        pool->queue_max_num = queue_max_num;
        pool->queue_cur_num = 0;
        pool->head = NULL;
        pool->tail = NULL;
        if(pthread_mutex_init(&pool->mutex, NULL) != 0)
        {
            printf("failed to init mutex!\n");
            break;
        }
        if(pthread_cond_init(&pool->queue_empty, NULL) != 0)
        {
            printf("failed to init queue_empty!\n");
            break;
        }
        if(pthread_cond_init(&pool->queue_not_empty, NULL) != 0)
        {
            printf("failed to init queue_not_empty!\n");
            break;
        }
        if(pthread_cond_init(&pool->queue_not_full, NULL) != 0)
        {
            printf("failed to init queue_not_full!\n");
            break;
        }
        pool->pthreads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
        if(pool->pthreads == NULL)
        {
            printf("failed to malloc pthreads!\n");
            break;
        }
        pool->pool_close = 0;
        pool->queue_close = 0;

        for(int i=0; i<pool->thread_num; i++)
        {
            pthread_create(&(pool->pthreads[i]), NULL, (void *)threadpool_function, (void *)pool);
        }

        return pool;
    }while(0);

    return NULL;
}

int threadpool_add_job(struct threadpool *pool, void* (*callback_function)(void *arg), void *arg)
{
    assert(pool != NULL);
    assert(callback_function != NULL);
    assert(arg != NULL);

    pthread_mutex_lock(&(pool->mutex));
    while((pool->queue_cur_num == pool->queue_max_num) && !(pool->queue_close || pool->pool_close))
    {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->mutex)); //队列满时就等待
    }
    if(pool->queue_close || pool->pool_close)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    struct job *pjob = (struct job *)malloc(sizeof(struct job));
    if(pjob == NULL)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    pjob->callback_function = callback_function;
    pjob->arg = arg;
    pjob->next = NULL;
    if(pool->head == NULL)
    {
        pool->head = pjob;
        pool->tail = pjob;
        pthread_cond_broadcast(&(pool->queue_not_empty));   //队列不满时，有任务来时就通知线程池中的线程：队列非空
    }
    else
    {
        pool->tail->next = pjob;
        pool->tail = pjob;
    }
    pool->queue_cur_num++;
    pthread_mutex_unlock(&(pool->mutex));

    return 0;
}

int threadpool_destroy(struct threadpool *pool)
{
    assert(pool != NULL);
    pthread_mutex_lock(&pool->mutex);
    if(pool->queue_close || pool->pool_close)   //如果进程池或线程池已经关闭，则直接返回
    {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }

    pool->queue_close = 1;
    while(pool->queue_cur_num != 0)
    {
        pthread_cond_wait(&(pool->queue_empty), &(pool->mutex));    //等待队列为空
    }

    pool->pool_close = 1;
    pthread_cond_broadcast(&(pool->queue_not_empty));   //唤醒线程池中正在阻塞的线程
    pthread_cond_broadcast(&(pool->queue_not_full));    //唤醒添加任务中threadpool_add_job()
    for(int i=0; i<pool->thread_num; i++)
    {
        pthread_join(pool->pthreads[i], NULL);  //等待线程池中所有线程执行完毕
    }
    //清理资源
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->queue_empty));
    pthread_cond_destroy(&(pool->queue_not_full));
    pthread_cond_destroy(&(pool->queue_not_empty));
    free(pool->pthreads);
    struct job *p;
    while(pool->head != NULL)
    {
        p = pool->head;
        pool->head = p->next;
        free(p);
    }
    free(pool);

    return 0;
}

void* threadpool_function(void *arg)
{
    struct threadpool *pool = (struct threadpool *)arg;
    struct job *pjob = NULL;
    while(1)
    {
        pthread_mutex_lock(&(pool->mutex));
        //队列为空时就等待
        while((pool->queue_cur_num == 0) && (pool->pool_close == 0))    //队列为空时就等待
        {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->mutex));
        }
        if(pool->pool_close)    //如果线程关闭，就退出
        {
            pthread_mutex_unlock(&(pool->mutex));
            pthread_exit(NULL);
        }
        pool->queue_cur_num--;
        pjob = pool->head;
        if(pool->queue_cur_num == 0)
        {
            pool->head = NULL;
            pool->tail = NULL;
        }
        else
        {
            pool->head = pjob->next;
        }

        pthread_mutex_unlock(&(pool->mutex));
        (*(pjob->callback_function))(pjob->arg);
        free(pjob);
        pjob = NULL;
    } 
}