#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>

#define FIFO_NAME "myfifo"
#define BUF_SIZE 1024

int main()
{
    int fd;
    char buf[BUF_SIZE] = "hello";

    umask(0);

    if(mkfifo(FIFO_NAME, __S_IFIFO | 0666) == -1)
    {
        perror("mkfifo error\n");
        exit(1);
    }

    if((fd = open(FIFO_NAME, O_WRONLY)) == -1)
    {
        perror("fopen error!\n");
        exit(1);
    }

    write(fd, buf, strlen(buf)+1);

    close(fd);
    exit(0);

    return 0;
}