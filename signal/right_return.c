#include<stdio.h>
#include<signal.h>
#include<setjmp.h>
#include <unistd.h>

#define ENV_UNSAVE 0
#define ENV_SAVED  1

int flag_saveenv = ENV_UNSAVE;
jmp_buf env;

void handler_sigrtmin15(int signo);
void handler_sigrtmax14(int signo);

int main()
{
    //设置返回点
    switch (sigsetjmp(env, 1))
    {
    case 0:
        printf("return 0\n");
        flag_saveenv = ENV_SAVED;
        break;
    case 1:
        printf("return from SIGRTMIN+15\n");
        break;
    case 2:
        printf("return form SIGRTMAX-15\n");
        break;
    default:
        printf("return else\n");
        break;
    }

    signal(SIGRTMIN+15, handler_sigrtmin15);
    signal(SIGRTMAX-14, handler_sigrtmax14);

    while(1);

    return 0;
}

void handler_sigrtmin15(int signo)
{
    if(flag_saveenv == ENV_UNSAVE)
    {
        return;
    }

    printf("recv SIGRTMIN+15\n");
    sleep(10);
    printf("in handler_sigrtmin15, after sleep\n");
    siglongjmp(env, 1);
}

void handler_sigrtmax14(int signo)
{
    if(flag_saveenv == ENV_UNSAVE)
    {
        return;
    }

    printf("recv SIGRTMAX-14\n");
    sleep(10);
    printf("in handler_sigrtmax14, after sleep\n");
    siglongjmp(env, 2);
}