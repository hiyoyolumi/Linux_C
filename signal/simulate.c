#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void send_ip();
void recv_ip();
void handler_sigalarm(int signo);

int main()
{
    //安装信号处理程序
    signal(SIGALRM, handler_sigalarm);

    //触发一个SIGALRM信号给本进程
    raise(SIGALRM);
    recv_ip();

    return 0;
}

void send_ip()
{
    //发送回请求报文，这里只是打印信息
    printf("send a icmp echo request packet\n");
}

void recv_ip()
{
    //挂起在套接字上等待数据并解析报文，这里只是使用死循环
    while(1);
}

void handler_sigalarm(int signo)
{
    send_ip();
    alarm(2);
}