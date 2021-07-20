#include "func.h"

void welcome()
{
    printf("[1] 登陆\n");
    printf("[2] 注册\n");
    printf("[3] 找回密码\n");
}

void welcome_1()
{
    printf("[1] 好友列表");
    printf("[2] 添加好友");
    printf("[3] 群列表");
    printf("[4] 群选项");
    printf("[5] 发送文件");
    printf("[6] 退出登陆");
}

void welcome_friends()
{
    

}

void Write(int fd, const char *buf)
{
    if(write(fd, buf, strlen(buf)) == -1)
    {
        my_err("write error", __LINE__);
    }
}






int get_userinfo(char *buf, int len)
{
    int i = 0;
    char c;
    
    if(buf == NULL)
        my_err("buf is NULL", __LINE__);

    while(((c = getchar()) != '\n') && (c != EOF) && (i < len - 2))
    {
        buf[i++] = c;
    }
    buf[i++] = '\n';
    buf[i++] = '\0';

    return 0;
}

void judge_userinfo(int fd, const char *string)
{
    char input_buf[32];
    char buf[BUFSIZ];
    char flag_userinfo = 'n';

    do
    {
        printf("%s:", string);
        
        get_userinfo(input_buf, sizeof(input_buf));
        //将用户输入发送到服务器
        write(fd, input_buf, strlen(input_buf));
        //读取服务器比对结果
        read(fd, buf, sizeof(buf));

        if(buf[0] == flag_userinfo)
            flag_userinfo = VALID_USERINFO;
        else
        {
            printf("%s error, input again:", string);
            flag_userinfo = INVALID_USERINFO;
        }

    } while (flag_userinfo == INVALID_USERINFO);

}

void my_err(const char *str, const int line)
{
    fprintf(stderr, "%d : %s : %s", line, str, strerror(errno));
    exit(1);
}




//处理注册请求
void *func_zhuce(void *arg)
{
    
}