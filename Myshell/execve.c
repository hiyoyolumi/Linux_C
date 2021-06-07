#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

int main(int argc, char *argv[], char **__environ)
{
    pid_t pid;
    int stat_val;

    pid = fork();
    switch (pid)
    {
    case -1:
        printf("process create failed.\n");
        exit(1);
    case 0:
        printf("Child process is running.\n");
        execve("processimage", argv, __environ);
        exit(0);
    
    default:
        printf("parent process is running.\n");
        break;
    }

    wait(&stat_val);
    exit(0);
}