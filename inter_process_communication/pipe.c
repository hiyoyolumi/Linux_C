#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void read_from_pipe(int fd);
void write_to_pipe(int fd);

int main()
{
    pid_t pid;
    int fd[2];
    int stat_val;

    //创建管道pipe()
    if (pipe(fd))
    {
        printf("create pipe failed.\n");
        exit(1);
    }

    //必须在fork()之前创建管道，否则子进程将不会继承管道的文件描述符
    pid = fork();
    switch (pid)
    {
    case -1:
        printf("fork error.\n");
        exit(1);
    case 0:
        //子进程关闭fd1
        close(fd[1]);
        read_from_pipe(fd[0]);
        exit(0);
    default:
        //父进程关闭fd0
        close(fd[0]);
        write_to_pipe(fd[1]);
        wait(&stat_val);
        exit(0);
    }

    return 0;
}

void read_from_pipe(int fd)
{
    char message[100];
    read(fd, message, 100);
    printf("read from pipe: %s", message);
}

void write_to_pipe(int fd)
{
    char *message = "Hello pipe!\n";
    write(fd, message, strlen(message) + 1);
}