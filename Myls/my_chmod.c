#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc, char ** argv)
{
    int mode;
    int mode_u;
    int mode_g;
    int mode_o;
    char *path;

    if(argc < 3)
    {
        printf("%s <mode num> <target file>", argv[0]);
        exit(0);        //exit包含在头文件<stdlib.h>中
    }

    mode = atoi(argv[1]);
    if(mode>777 || mode<0)
    {
        printf("mode run error!\n");
        exit(0);
    }

    mode_u = mode/100;
    mode_g = (mode - mode_u*100)/10;
    mode_o = mode%10;

    mode = (mode_u * 8 * 8) + (mode_g * 8) + mode_o;
    path = argv[2];

    if(chmod(path, mode) == -1)
    {
        perror("chmod error");
        exit(1);
    }

    return 0;
}
