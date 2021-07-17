#define MY_RECV_C

#include "my_server.h"

//服务器关闭返回0,出错返回-1,成功返回读取的字节数
int my_recv(int conn_fd, char * data_buf, int len)
{
    int i;
    static char recv_buf[BUFSIZE]; //缓冲区
    static char *pread;        //下一次读取数据的位置
    static int len_remain = 0; //自定义缓冲区中剩余的字节数
    
    //如果自定义缓冲区中没有数据，则从套接字读取数据
    if(len_remain <= 0)
    {
        len_remain = recv(conn_fd, recv_buf, sizeof(recv_buf), 0);
        if(len_remain == -1)
        {
            printf("recv() error: %s\n", strerror(errno));
            exit(1);
        }
        else if(len_remain == 0)
        {
            return 0;
        }
        pread = recv_buf;
    }

    //从自定义缓冲区中读取一次数据
    for(i=0; *pread != '\n'; i++)
    {
        if(i > len)
        {
            return -1;
        }
        data_buf[i] = *pread++;
        len_remain--;
    }
    //去除结束标志'\n'
    pread++;
    len_remain--;

    return i;
}