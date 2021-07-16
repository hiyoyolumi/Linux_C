#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<errno.h>

#define BUF_SIZE 256
#define PROJ_ID 32
#define PATH_NAME "/tmp"
#define SERVER_MSG 1
#define CLIENT_MAG 2

int main()
{
    struct mymsgbuf
    {
        long msgtype;
        char ctrlstring[BUF_SIZE];
    }mybuffer;
    
    int qid;
    int msglen;
    key_t msgkey;

    if((msgkey = ftok(PATH_NAME, PROJ_ID)) == -1)
    {
        perror("ftok error\n");
        exit(1);
    }
    if((qid = msgget(msgkey, IPC_CREAT|0660)) == -1)
    {
        perror("msgget error\n");
        exit(1);
    }

    while(1)
    {
        printf("server:");
        fgets(mybuffer.ctrlstring, BUF_SIZE, stdin);
        if(strncmp("quit", mybuffer.ctrlstring, 4) == 0)
        {
            msgctl(qid, IPC_RMID, NULL);
            break;
        }
        mybuffer.ctrlstring[strlen(mybuffer.ctrlstring) - 1] = '\0';
        mybuffer.msgtype = SERVER_MSG;
        if((msgsnd(qid, &mybuffer, strlen(mybuffer.ctrlstring) + 1, 0)) == -1)
        {
            perror("Server msg error\n");
            exit(1);
        }
        if((msgrcv(qid, &mybuffer, BUF_SIZE, CLIENT_MAG, 0)) == -1)
        {
            perror("msgrcv error\n");
            exit(1);
        }
        printf("client: %s\n", mybuffer.ctrlstring);
    }
    exit(0);

    return 0;
}