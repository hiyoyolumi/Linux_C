#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>

#define FIFO_READ "readfifo"
#define FIFO_WRITE "writefifo"
#define BUF_SIZE 1024


int main()
{
    int wfd, rfd;
    char buf[BUF_SIZE];
    int len;

    umask(0);
    if(mkfifo(FIFO_WRITE, __S_IFIFO| 0666) == -1)
    {
        printf("can not create fifo %s, because %s\n", FIFO_WRITE, strerror(errno));
        exit(1);
    }

    umask(0);
    wfd = open(FIFO_WRITE, O_WRONLY);
    if(wfd == -1)
    {
        //printf("hello\n");
        printf("open fifo %s errno:%s\n", FIFO_WRITE, strerror(errno));
        exit(1);
    }

    while((rfd = open(FIFO_READ, O_RDONLY)) == -1)
    {
        sleep(1);
    }

    while(1)
    {
        printf("Server:");
        fgets(buf, BUF_SIZE, stdin);
        buf[strlen(buf) - 1] = '\0';
        if(strcmp(buf, "quit") == 0)
        {
            close(wfd);
            unlink(FIFO_WRITE);
            close(rfd);
            exit(0);
        }
        write(wfd, buf, strlen(buf));


        len = read(rfd, buf, BUF_SIZE);
        buf[len] = '\0';
        printf("Client:%s\n", buf);
    }

    return 0;
}