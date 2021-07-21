#include "func.h"

#define SERV_PORT 4507
#define MAXEVE 1024

int num_birth;

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

    pthread_t thid[1000];
    int z = 0;
    // int flag = 0;   //线程id的下标

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

    //建表
//     query_str = "create table UserData(username varchar(20), password varchar(20),\
// nickname varchar(20), mibao varchar(20), num double)";
//     rc = mysql_real_query(&mysql, query_str, strlen(query_str));
//     if(rc != 0)
//         my_err("mysql_real_query", __LINE__);

    query_str = "select * from UserData";
    rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    if(rc != 0)
        my_err("mysql_real_query error", __LINE__);
    res = mysql_store_result(&mysql);
    if(res == NULL)
        my_err("mysql_store_query", __LINE__);
    rows = mysql_num_rows(res);
    fields = mysql_num_fields(res);
    //每次启动服务器时，检索UserData中num的最大值并赋值给num_birth
    while(row = mysql_fetch_row(res))
    {
        for(i=0; i<fields; i++)
        {
            num_birth = atoi(row[fields - 1]);
        }
    }
    num_birth += 1;


    //初始化服务器
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
        my_err("socket error", __LINE__);

    int optval = 1;
    if(setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) < 0)
        my_err("setsockopt error", __LINE__);

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

    printf("---initializing...\n");
    printf("---loading finished\n");

    //int client[2];      //存两个客户端的fd
    struct cfd_mysql cm;

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
                request[strlen(request)-1] = '\0';
                if(n == 0)
                {
                    close(ep[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep[i].data.fd, NULL);
                }
                else if(n > 0)
                {
                    if(write(STDOUT_FILENO, buf, n) == -1)
                        my_err("write orror", __LINE__);

                    //接受客户端发来的请求，并调用相关函数
                    //strcpy(request, buf);
                    //request[strlen(request)-1] = '\0';
                    if(strncmp(buf, "2\n", 2) == 0)
                    {
                        cm.cfd = ep[i].data.fd;
                        cm.mysql = mysql;
                        cm.clit_addr = clit_addr;
                        strcpy(buf, "---开始注册\n");
                        Write(ep[i].data.fd, buf);
                        if(pthread_create(&thid[z++], NULL, func_zhuce, (void *)&cm) == -1)
                            my_err("pthread_create error\n", __LINE__);
                        pthread_join(thid[z - 1], NULL);
                        Write(ep[i].data.fd, "---注册成功\n");

                        Write(cm.cfd, "[1] 登陆\n[2] 注册\n[3] 找回密码\n");
                    }
                    else if(strncmp(buf, "1\n", 2) == 0)
                    {
                        
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


    for(i=0; i<100; i++)
        pthread_join(thid[i], NULL);

    for(i=0; i<ret; i++)
        close(ep[i].data.fd);

    return 0;
}