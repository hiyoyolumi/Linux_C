#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>

#define BUF_SIZE 256
#define PATH_NAME "."
#define PROJ_ID 30

int main()
{
    struct mymsgbuf
    {
        long msgtype;
        char ctrlstring[BUF_SIZE];
    };
    struct mymsgbuf msgbuffer;
    key_t msgkey;
    int qid;

    if((msgkey = ftok(PATH_NAME, PROJ_ID)) == -1)
    {
        printf("ftok error\n");
        exit(1);
    }
    printf("msgkey = %u\n", msgkey);
    if((qid = msgget(msgkey, 0777 | IPC_CREAT)) == -1)
    {
        printf("msgget error:%s\n", strerror(errno));
        exit(1);
    }
    //qid = msgget(msgkey, IPC_CREAT|0666);

    msgbuffer.msgtype = 3;
    strcpy(msgbuffer.ctrlstring, "hello message queue");
    if(msgsnd(qid, msgbuffer.ctrlstring, sizeof(msgbuffer.ctrlstring)-4, 0) == -1)
    {
        printf("msgsnd error");
        exit(1);
    }
    exit(0);

    return 0;
}