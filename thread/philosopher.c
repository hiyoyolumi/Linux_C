#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5

 
//某位哲学家开始拿第一支筷子时，其他哲学家全部不准拿筷子（即使他已经饿了），
//只可以放下，直到这位哲学家拿到一双筷子之后，允许其他筷子被拿起


sem_t chopsticks[N]; //筷子的编号

pthread_mutex_t mutex; //定义互斥锁

int philosophers[N] = {0, 1, 2, 3, 4}; //哲学家的编号

void *philosopher(void *arg);
void delay(int len);

int main()
{
    pthread_t philo[N];
    srand(time(NULL));

    //初始化信号量
    for (int i = 0; i < N; i++)
    {
        sem_init(&chopsticks[i], 0, 1);
    }

    //初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    //创建线程
    for (int i = 0; i < N; i++)
    {
        pthread_create(&philo[i], NULL, (void *)philosopher, &philosophers[i]);
    }

    //挂起线程
    for (int i = 0; i < N; i++)
    {
        pthread_join(philo[i], NULL);
    }

    //销毁信号量
    for (int i = 0; i < N; i++)
    {
        sem_destroy(&chopsticks[i]);
    }

    //销毁互斥锁
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_destroy(&mutex);
    }

    return 0;
}

void *philosopher(void *arg)
{
    int i = *(int *)arg;
    int left = i; //左筷子的编号和哲学家的编号相同
    int right = (i + 1) % N;
    while (1)
    {
        printf("Philosophy %d is thinking.\n", i);
        delay(60000);

        printf("Philosophy %d is hungry.\n", i);

        //加锁
        pthread_mutex_lock(&mutex);

        sem_wait(&chopsticks[left]); //当这个哲学家的左筷子存在时，可以继续,否则等待
        printf("Now philosophy %d had chopstick %d, He have only one, Can Not Eat!\n", i, left);
        sem_wait(&chopsticks[right]);
        printf("philosophy %d had chopstick %d, He have a pair of chopsticks, He Can Eat!\n", i, right);

        //解锁
        pthread_mutex_unlock(&mutex);

        printf("philosophy %d begin eating!\n", i);
        delay(60000);
        sem_post(&chopsticks[left]);
        sem_post(&chopsticks[right]);
        printf("phylosophy %d lay down chopstick %d & %d\n", i, left, right);
    }
}

void delay(int len)
{
    int i = rand() % len;
    int x;
    while (i > 0)
    {
        x = rand() % len;
        while (x > 0)
        {
            x--;
        }
        i--;
    }
}