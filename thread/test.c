#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>

struct node
{
    int data;
    struct node *next;
};

pthread_mutex_t mutex;
pthread_cond_t cond;
struct node *head;

void *produce(void *arg)
{
    struct node *prod;
    struct node *t;
    while(1)
    {
        t = head;
        prod = (struct node *)malloc(sizeof(struct node));
        prod->data = rand()%5;
        pthread_mutex_lock(&mutex);
        prod->next = head;
        head = prod;
        while(t)
        {
            printf("%d-->", t->data);
            t = t->next;
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        printf("---producer: %d\n", head->data);

        sleep(rand() % 3);
    }

    return NULL;
}

void *consume(void *arg)
{
    struct node *mp;
    struct node *t;
    while(1)
    {
        t = head;
        pthread_mutex_lock(&mutex);
        while(head->next == NULL)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        while(t)
        {
            printf("%d-->", t->data);
            t = t->next;
        }
        printf("\n");
        mp = head;
        head = mp->next;
        pthread_mutex_unlock(&mutex);
        free(mp);
        printf("---------consumer %d\n", head->data);

        sleep(rand() % 3);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_t proid, conid;
    pthread_create(&proid, NULL, produce, NULL);
    pthread_create(&conid, NULL, consume, NULL);

    pthread_join(proid, NULL);
    pthread_join(conid, NULL);

    return 0;
}