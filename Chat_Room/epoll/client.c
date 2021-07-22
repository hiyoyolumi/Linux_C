#include "func.h"

#define SERV_PORT 4507

int num_birth;
int clients[1000];
pthread_mutex_t mutex;

int lfd;
int epfd;
struct epoll_event tep, ep[MAXEVE];

int level;

void *my_write(void *arg)
{
    char buf[BUFSIZ];
    int cfd = *(int *)arg;
    //printf("this my_write cfd = %d\n", cfd);

    while(1)
    {
        //printf("this my_write cfd = %d\n", cfd);
        //scanf("%s", buf);
        get_userinfo(buf, sizeof(buf));
        write(cfd, buf, strlen(buf));
    }

    return NULL;
}

void *my_read(void *arg)
{
    int len;
    char buf[BUFSIZ];
    int cfd = *(int *)arg;
    //printf("this my_read cfd = %d\n", cfd);

    while(1)
    {
        len = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
        //printf("\n");
    }

    return NULL;
}





int main()
{
    int cfd;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZ];
    int len;

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(cfd == -1)
        my_err("socket error", __LINE__);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    if(connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        my_err("connect error", __LINE__);
    

    //打印服务器发来的欢迎信息
    read(cfd, buf, sizeof(buf));
    write(STDOUT_FILENO, buf, strlen(buf));

    //welcome_interface(cfd);
    //打印欢迎界面信息
    welcome();

    

    pthread_t wthid, rthid;

    if(pthread_create(&wthid, NULL, my_write, (void *)&cfd) == -1)
        my_err("pthread_create error", __LINE__);
    if(pthread_create(&rthid, NULL, my_read, (void *)&cfd) == -1)
        my_err("pthread_create error", __LINE__);
    //printf("hello\n");
    /*
    while(1)
    {
        scanf("%s", buf);
        write(cfd, buf, strlen(buf));
        //send(cfd, buf, strlen(buf), 0);

        len = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
        printf("\n");
        //len = recv(cfd, buf, sizeof(buf), 0);
        //write(STDOUT_FILENO, buf, len);
        //printf("\n");
    }
    */

    pthread_join(wthid, NULL);
    pthread_join(rthid, NULL);

    close(cfd);

    return 0;
}