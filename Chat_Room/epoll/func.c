#include "func.h"

extern int num_birth;

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

void Read(int fd, void *buf, size_t count)
{
    if(read(fd, buf, sizeof(buf)) == -1)
    {
        my_err("read error", __LINE__);
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
    struct node
    {
        char username[20];
        char password[20];
        char nickname[20];
        char mibao[20];
        int num;
    };

    struct node data;
    struct cfd_mysql cm;
    cm = *(struct cfd_mysql *)arg;
    char buf[BUFSIZ];
    int ret;

    char query_str[200];

    while(1)
    {
        Write(cm.cfd, "---请输入账号:");
        read(cm.cfd, buf, sizeof(buf));
        buf[strlen(buf)-1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 1);
        if(ret == 0)
        {
            // strcpy(buf, "---用户名已存在，请重新输入:\n");
            Write(cm.cfd, "---用户名已存在，请重新输入:\n");
        }
        else
        {
            //得到一个用户的所有4个信息
            strcpy(data.username, buf);

            Write(cm.cfd, "---请输入密码:");
            // strcpy(buf, "---请输入密码:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            buf[strlen(buf)-1] = '\0';
            strcpy(data.password, buf);

            Write(cm.cfd, "---请输入昵称:");
            // strcpy(buf, "---请输入昵称:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            buf[strlen(buf)-1] = '\0';
            strcpy(data.nickname, buf);

            Write(cm.cfd, "---请输入密保:");
            // strcpy(buf, "---请输入密保:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            buf[strlen(buf)-1] = '\0';
            strcpy(data.mibao, buf);

            data.num = num_birth++;

            //向UserData中add数据
            sprintf(query_str, "\
insert into UserData values(\"%s\", \"%s\", \"%s\", \"%s\", \"%d\")", \
data.username, data.password, data.nickname, data.mibao, data.num);
            printf("%s", query_str);
            mysql_add(&cm.mysql, query_str, cm.clit_addr, "UserData");

            break;
        }
    }
    pthread_exit(0);
}

// insert into UserData values("")