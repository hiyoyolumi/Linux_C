#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t empty, full;      //全局同步信号量    empty表示缓冲区空余量   full表示缓冲区数据量
pthread_mutex_t mutex;  //全局互斥变量
int buffer_count = 0;   //表示管道内的产品数目

void *producer(void *arg);
void *consumer(void *arg);

int main()
{
    pthread_t thrd_prod, thrd_cons;

    //初始化
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, 5);
    sem_init(&full, 0, 0);

    //创建生产者和消费者线程
    if (pthread_create(&thrd_prod, NULL, (void *)producer, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }
    if (pthread_create(&thrd_cons, NULL, consumer, NULL) != 0)
    {
        printf("thread create failed.\n");
        exit(1);
    }

    //等待线程结束
    if (pthread_join(thrd_prod, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }
    if (pthread_join(thrd_cons, NULL) != 0)
    {
        printf("thread wait failed.\n");
        exit(1);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *producer(void *arg)
{
    while (1)
    {
        //当没有缓冲区没有空位置(即empty==0)时，sem_wait()等待，直到信号量不为0
        sem_wait(&empty);           //empty-1
        pthread_mutex_lock(&mutex); //加锁

        //成功占有互斥量，接下来可以对缓冲区（仓库）进行生产
        buffer_count++;
        printf("producer put a product to buffer. the buffer count is %d\n", buffer_count);
        pthread_mutex_unlock(&mutex); //解锁
        sem_post(&full);              //full+1
    }
}

void *consumer(void *arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        buffer_count--;
        printf("consumer get a product from buffer. the buffer count is %d\n", buffer_count);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}