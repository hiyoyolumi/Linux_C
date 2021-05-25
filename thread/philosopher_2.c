#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>

#define N 5

void delay(int len);
void *philosopher(void *arg);


    //规定奇数号哲学家先拿左筷子再拿右筷子，而偶数号哲学家相反。
    //所以将是 2，3 号哲学家竞争 3 号筷子，4，5 号哲学家竞争 5 号筷子。


sem_t chopsticks[N];    //定义5种资源
int philosophers[N] = {0, 1, 2, 3, 4};  //定义5位哲学家的编号
//规定编号奇数的哲学家先拿左手边的筷子

int main()
{
    pthread_t philo[N];
    srand(time(NULL));

    //初始化信号量
    for(int i=0; i<N; i++)
    {
        sem_init(&chopsticks[i], 0, 1);
    }

    //创建线程
    for(int i=0; i<N; i++)
    {
        pthread_create(&philo[i], NULL, (void *)philosopher, &philosophers[i]);
    }

    //挂起线程
    for(int i=0; i<N; i++)
    {
        pthread_join(philo[i], NULL);
    }

    //销毁信号量
    for(int i=0; i<N; i++)
    {
        sem_destroy(&chopsticks[i]);
    }

     return 0;
}

void *philosopher(void *arg)
{
    int i = *(int *)arg;
    int left = i;
    int right = (i+1) % N;

    while(1)
    {
        printf("philosophy %d is thinking.\n", i);
        delay(60000);
        printf("philosophy %d is hungry.\n", i);
        if(i % 2 != 0)  //编号为奇数
        {
            sem_wait(&chopsticks[left]);
            printf("Now philosophy %d had chopstick %d, He have only one, Can Not Eat!\n", i, left);
            sem_wait(&chopsticks[right]);
            printf("philosophy %d had chopstick %d, Now he have a pair of chopsticks, He Can Eat!\n", i, right);
            printf("philosophy %d begin eating!\n", i);
            delay(60000);
            sem_post(&chopsticks[left]);
            sem_post(&chopsticks[right]);
            printf("philosophy %d lay down chopsticks %d & %d", i, left, right);
        }
        else
        {
            sem_wait(&chopsticks[right]);
            printf("Now philosophy %d had chopstick %d, He have only one, Can Not Eat!\n", i, right);
            sem_wait(&chopsticks[left]);
            printf("philosophy %d had chopstick %d, Now he have a pair of chopsticks, He Can Eat!\n", i, left);
            printf("philosophy %d begin eating!\n", i);
            delay(60000);
            sem_post(&chopsticks[left]);
            sem_post(&chopsticks[right]);
            printf("philosophy %d lay down chopsticks %d & %d", i, left, right);
        }
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