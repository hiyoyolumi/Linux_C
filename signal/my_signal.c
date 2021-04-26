#include<stdio.h>
#include<signal.h>

void handler_sigint(int signo);

int main()
{
    //安装信号处理函数
    signal(SIGINT, handler_sigint);

    while(1);

    return 0;
}

void handler_sigint(int signo)
{
    printf("recv SIGINT\n");
}