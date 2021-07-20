#include "func.h"

#define SERV_PORT 4507
#define MAXEVE 1024



int main()
{
    int lfd, cfd;
    struct sockaddr_in serv_addr, clit_addr;
    int clit_addr_len;
    int ret;
    int i,j = 0;
    int n;
    char buf[BUFSIZ];
    char request[BUFSIZ];

    MYSQL           mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    char            *query_str = NULL;
    int             rc, fields;
    int             rows;

    pthread_t thid[100];
    int flag = 0;   //线程id的下标

    //初始化数据库
    if (NULL == mysql_init(&mysql)) {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
    if (NULL == mysql_real_connect(&mysql,
                "127.0.0.1",
                "root",
                "xjmwsb1234",
                "testdb",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("1. Connected MySQL successful! \n");



    //初始化服务器
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
        my_err("socket error", __LINE__);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        my_err("bind error", __LINE__);

    if(listen(lfd, 128) == -1)
        my_err("listen error", __LINE__);

    /*clit_addr_len = sizeof(clit_addr);
    cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
    if(cfd == -1)
        my_err("accept error", __LINE__);*/

    int epfd = epoll_create(MAXEVE);
    struct epoll_event tep, ep[MAXEVE]; //tep用来设置单个属性，ep是epoll_wait()传出满足事件的数组
    tep.data.fd = lfd;
    tep.events = EPOLLIN;

    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &tep);

    //int client[2];      //存两个客户端的fd

    while(1)
    {   
        //等待客户端的连接请求
        ret = epoll_wait(epfd, ep, MAXEVE, -1);
        for(i=0; i<ret; i++)
        {
            if(ep[i].data.fd == lfd)    //lfd满足读事件，有新的客户端发起连接请求
            {
                clit_addr_len = sizeof(clit_addr);
                cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
                if(cfd == -1)
                    my_err("accept error", __LINE__);

                tep.data.fd = cfd;
                tep.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tep);

                //
                //client[j++] = cfd;

                printf("ip %s is connect\n", inet_ntoa(clit_addr.sin_addr));
                Write(cfd, "Welcome to my_server!\n");
            }
            else                        //cfd们满足读事件，有客户端数据写来
            {
                //读套接字
                n = read(ep[i].data.fd, buf, sizeof(buf));
                if(n == 0)
                {
                    close(ep[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep[i].data.fd, NULL);
                }
                else if(n > 0)
                {
                    // if(write(STDOUT_FILENO, buf, n) == -1)
                    //     my_err("write orror", __LINE__);

                    //接受客户端发来的请求，并调用相关函数
                    strcpy(request, buf);
                    if(strcmp(request, "zhuce") == 0)
                    {
                        if(pthread_create(&thid[i], NULL, func_zhuce, (void *)&ep[i].data.fd) == -1)
                            my_err("pthread_create error\n", __LINE__);
                    }
                    else
                    {

                    }




                    // if(client[0] == ep[i].data.fd)
                    // {
                    //     if(write(client[1], buf, n) == -1)
                    //         my_err("write error", __LINE__);
                    // }
                    // else if (client[1] == ep[i].data.fd)
                    // {
                    //     if(write(client[0], buf, n) == -1)
                    //         my_err("write error", __LINE__);
                    // }
                    
                }
                else
                {
                    my_err("read error", __LINE__);
                }
            }
        }
    }

    for(i=0; i<n; i++)
    {
        close(ep[i].data.fd);
    }

    return 0;
}