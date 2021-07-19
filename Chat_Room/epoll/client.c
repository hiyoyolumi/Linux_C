#include "func.h"

#define SERV_PORT 4507

void my_err(const char *str, const int line)
{
    fprintf(stderr, "%d : %s : %s", line, str, strerror(errno));
    exit(1);
}

void *my_write(void *arg)
{
    char buf[BUFSIZ];
    int cfd = *(int *)arg;
    //printf("this my_write cfd = %d\n", cfd);

    while(1)
    {
        //printf("this my_write cfd = %d\n", cfd);
        scanf("%s", buf);
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
        printf("\n");
    }

    return NULL;
}



void welcome_interface(int cfd)
{
    int a;

    //打印软件欢迎界面信息
    welcome();
    //输入选项
    scanf("%d", &a);
    switch(a)
    {
        case 1:     //登陆
            //输入用户名
            judge_userinfo(cfd, "username");
            //输入用户密码
            judge_userinfo(cfd, "password");
            printf("login success!\n");
            //进入用户界面
            client_interface();

        case 2:     //注册
            //输入用户名

            //输入用户密码

        case 3:     //找回密码
            //输入用户名

            //输入新密码


    }
}

void client_interface()
{
    int a;

    //打印用户界面信息
    welcome_1();
    scanf("%d", &a);
    switch(a)
    {
        case 1:     //好友列表
            //打印出自己的好友 及选项
            friends_interface();

        case 2:     //添加好友
            //输入要添加的好友的账号，发送给服务器

        case 3:     //群列表
            //打印出自己加入的群

        case 4:     //群选项
            //打印出群界面
        
        case 5:     //发送文件
            //未知
    }
}

void friends_interface()
{
    //打印好友列表
    welcome_friends();
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

    /*
    int a;  //保存用户的输入选项
    //打印用户界面
    welcome();
    //输入选项
    scanf("%d", &a);
    switch(a)
    {
        case 1:     //登陆
            //输入用户名
            judge_userinfo(cfd, "username");
            //输入用户密码
            judge_userinfo(cfd, "password");
            printf("login success!\n");
            //打印用户界面
            welcome_1();

    }
    */

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