#include "func.h"

#define SERV_PORT 4507
#define MAXEVE 1024

void my_err(const char *str, const int line)
{
    fprintf(stderr, "%d : %s : %s", line, str, strerror(errno));
    exit(1);
}

int main()
{
    int lfd, cfd;
    struct sockaddr_in serv_addr, clit_addr;
    int clit_addr_len;
    int ret;
    int i,j = 0;
    int n;
    char buf[BUFSIZ];

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

    int client[2];      //存两个客户端的fd

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
                client[j++] = cfd;

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
                    if(write(STDOUT_FILENO, buf, n) == -1)
                        my_err("write orror", __LINE__);
                    printf("\n");

                    /*
                    //小-->大
                    for(j=0; j<n; j++)
                        buf[j] = toupper(buf[j]);
                    */


                    /*if(write(ep[i].data.fd, buf, n) == -1)
                        my_err("write error", __LINE__);\
                    */
                    if(client[0] == ep[i].data.fd)
                    {
                        if(write(client[1], buf, n) == -1)
                            my_err("write error", __LINE__);
                    }
                    else if (client[1] == ep[i].data.fd)
                    {
                        if(write(client[0], buf, n) == -1)
                            my_err("write error", __LINE__);
                    }
                    
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