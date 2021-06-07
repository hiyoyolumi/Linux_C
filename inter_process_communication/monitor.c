#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

int main(int argc, char *argv[], char **__environ)
{
    int fd[2];
    int stat_val;
    pid_t pid;

    if(argc < 2)
    {
        printf("Wrong parameters.\n");
        exit(0);
    }
    if(pipe(fd))
    {
        printf("Create pipe failed.\n");
        exit(1);
    }
    
    pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork failed.\n");
        exit(1);
    case 0:
        //关闭子进程的标准输入 #define	STDIN_FILENO	0
        close(0);
        //复制管道输入端到标准输入
        dup(fd[0]);
        execve("ctrlpocess", argv, __environ);
        exit(0);
    
    default:
        close(fd[0]);
        //将命令行第一个参数写进管道里
        write(fd[1], argv[1], strlen(argv[1]));
        break;
    }

    wait(&stat_val);

    exit(0);
}