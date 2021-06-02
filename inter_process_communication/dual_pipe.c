#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void ch_read_write_pipe(int fdr, int fdw);
void pa_read_write_pipe(int fdr, int fdw);

int main()
{
    int fd1[2], fd2[2];
    pid_t pid;
    int stat_val;

    if (pipe(fd1) || pipe(fd2))
    {
        printf("pipe fd1 create failed!\n");
        exit(1);
    }

    pid = fork();
    switch (pid)
    {
        case -1:
            printf("fork error!\n");
            exit(1);
        case 0:
            close(fd1[1]); //可读
            close(fd2[0]); //可写
            ch_read_write_pipe(fd1[0], fd2[1]);
            exit(0);

        default:
            close(fd1[0]); //可写
            close(fd2[1]); //可读
            pa_read_write_pipe(fd2[0], fd1[1]);
            wait(&stat_val);
            exit(0);
    }

    return 0;
}

void ch_read_write_pipe(int fdr, int fdw)
{
    //printf("hello from ch!\n");
    char read_message[100];
    char *write_message = "from child process!\n";

    //先写再读
    write(fdw, write_message, strlen(write_message) + 1);
    read(fdr, read_message, 100);

    printf("child process read  %s", read_message);
}

void pa_read_write_pipe(int fdr, int fdw)
{
    //printf("hello from pa!\n");
    char read_message[100];
    char *write_message = "from parent process!\n";

    write(fdw, write_message, strlen(write_message) + 1);
    read(fdr, read_message, 100);

    printf("parent process read %s", read_message);
}