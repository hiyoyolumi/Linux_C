#include "func.h"

extern int num_birth;

char *get_time(char *now_time)
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strcpy(now_time, asctime (timeinfo));
	now_time[strlen(now_time)-1] = '\0';

    return now_time;
}

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
        {
            flag_userinfo = VALID_USERINFO;
        }
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



//处理登陆请求
void *func_denglu(void *arg)
{
    struct node data;
    struct cfd_mysql cm;
    struct cfd_mysql *retval;
    retval = (struct cfd_mysql *)malloc(sizeof(struct cfd_mysql));
    cm = *(struct cfd_mysql *)arg;
    retval->cfd = cm.cfd;
    retval->mysql = cm.mysql;
    retval->clit_addr = cm.clit_addr;
    retval->retval = 1;

    char buf[BUFSIZ];
    int ret = 0;
    int n;
    char now_time[100];

    while(1)
    {
        Write(cm.cfd, "---请输入账号(q to quit):");
        n = read(cm.cfd, buf, sizeof(buf));
        retval->retval = huitui_val(buf);
        pthread_exit((void *)retval);
        buf[n - 1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 1);
        if(ret == 0)
        {
            printf("[%s] ip %s username input correct\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            strcpy(retval->username, buf);
            break;
        }
        else
        {
            printf("[%s] ip %s username input error\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---账号不存在\n");
            continue;
        }
    }
    int rows;
    MYSQL_ROW row;
    MYSQL_RES *res;
    while(1)
    {
        Write(cm.cfd, "---请输入密码(q to quit):");
        n = read(cm.cfd, buf, sizeof(buf));
        retval->retval = huitui_val(buf);
        pthread_exit((void *)retval);
        buf[n - 1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 2);
        if(ret == 0)
        {
            printf("[%s] ip %s input correct password\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---正在登陆...\n");
            char temp[100];
            sprintf(temp, "update UserData set status = 1 where username = \"%s\"", retval->username);
            rows = mysql_real_query(&retval->mysql, temp, strlen(temp));
            if(rows != 0)
                my_err("mysql_real_query error", __LINE__);
            res = mysql_store_result(&retval->mysql);
            if(res == NULL)
                my_err("mysql_store_result error", __LINE__);
            while(row = mysql_fetch_row(res))
            {
                //将该用户 设置为登陆状态
                if(strcmp(retval->username, row[0]) == 0)
                {
                    row[5] = 1;
                }
            }
            sleep(1);
            //Write(cm.cfd, "---登陆成功\n");
            break;
        }
        else
        {
            printf("[%s] ip %s input error password\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---密码错误\n");
            continue;
        }
    }

    pthread_exit((void *)retval);
    //pthread_exit((void *)1);
}

//处理注册请求
void *func_zhuce(void *arg)
{
    // struct node
    // {
    //     char username[20];
    //     char password[20];
    //     char nickname[20];
    //     char mibao[20];
    //     int num;
    // };

    struct node data;
    struct cfd_mysql cm;
    cm = *(struct cfd_mysql *)arg;
    char buf[BUFSIZ];
    int ret;

    char query_str[200];
    char now_time[100];

    while(1)
    {
        Write(cm.cfd, "---请输入账号(q to quit):");
        read(cm.cfd, buf, sizeof(buf));
        huitui(buf);
        // if(strncmp(buf, "q\n", 2) == 0)
        // {
        //     printf("cancel zhuce\n");
        //     pthread_exit((void *)0);
        // }
        buf[strlen(buf)-1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 1);
        if(ret == 0)
        {
            // strcpy(buf, "---用户名已存在，请重新输入:\n");
            Write(cm.cfd, "---用户名已存在，请重新输入:(q to quit)\n");
        }
        else
        {
            //得到一个用户的所有4个信息
            strcpy(data.username, buf);

            Write(cm.cfd, "---请输入密码(q to quit):");
            // strcpy(buf, "---请输入密码:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            huitui(buf);
            buf[strlen(buf)-1] = '\0';
            strcpy(data.password, buf);

            Write(cm.cfd, "---请输入昵称(q to quit):");
            // strcpy(buf, "---请输入昵称:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            huitui(buf);
            buf[strlen(buf)-1] = '\0';
            strcpy(data.nickname, buf);

            Write(cm.cfd, "---请输入密保(q to quit):");
            // strcpy(buf, "---请输入密保:");
            // Write(cm.cfd, buf);
            read(cm.cfd, buf, sizeof(buf));
            huitui(buf);
            buf[strlen(buf)-1] = '\0';
            strcpy(data.mibao, buf);

            data.num = num_birth++;

            //向UserData中add数据
            sprintf(query_str, "\
insert into UserData values(\"%s\", \"%s\", \"%s\", \"%s\", \"%d\", \"%d\")", \
data.username, data.password, data.nickname, data.mibao, data.num, 0);
            printf("[%s] %s", get_time(now_time), query_str);
            mysql_add(&cm.mysql, query_str, cm.clit_addr, "UserData");

            break;
        }
    }
    pthread_exit((void *)1);
}

void *func_zhaohui(void *arg)
{
    struct cfd_mysql cm;
    cm = *(struct cfd_mysql *)arg;
    char buf[BUFSIZ];
    int ret;
    char new[20];
    char username[20];
    char query_str[100];
    char now_time[100];

    while(1)
    {
        Write(cm.cfd, "---请输入账号(q to quit)\n");
        read(cm.cfd, buf, sizeof(buf));
        huitui(buf);
        buf[strlen(buf) - 1] = '\0';
        strcpy(username, buf);
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 1);
        if(ret == 0)
        {
            printf("[%s] ip %s username input correct\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            break;
        }
        else
        {
            printf("[%s] ip %s username input error\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---用户名不存在\n");
            continue;
        }
    }
    while(1)
    {
        Write(cm.cfd, "---请输入密保(q to quit)\n");
        read(cm.cfd, buf, sizeof(buf));
        huitui(buf);
        buf[strlen(buf) - 1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 4);
        if(ret == 0)
        {
            printf("[%s] ip %s mibao input correct\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---密保正确\n");
            while(1)
            {
                Write(cm.cfd, "---请输入新密码(q to quit):");
                read(cm.cfd, buf, sizeof(buf));
                huitui(buf);
                buf[strlen(buf) - 1] = '\0';

                Write(cm.cfd, "---再次输入密码(q to quit):");
                read(cm.cfd, new, sizeof(new));
                huitui(new);
                new[strlen(new) - 1];
                if(strncmp(new, buf, strlen(buf)) == 0)
                {
                    printf("[%s] ip %s username change password\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
                    printf("new password:%s\n", new);
                    sprintf(query_str, "update UserData set password = %s where username = \"%s\"", new, username);
                    Write(cm.cfd, "---修改密码成功\n");
                    break;
                }
                else
                {
                    Write(cm.cfd, "---两次密码输入不相同\n");
                    continue;
                }
            }
            break;
        }
        else
        {
            printf("[%s] ip %s username input error\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---密保错误\n");
            continue;
        }
    }
    pthread_exit((void *)1);
}

int huitui_val(const char *buf)
{
    if(strncmp(buf, "q\n", 2) == 0) 
    {
        printf("cancel\n");
        return 0;
    }
}

int huitui(const char *buf)
{
    if(strncmp(buf, "q\n", 2) == 0) 
    {
        printf("cancel\n");
        pthread_exit((void *)0);
    }
}


//分离状态的线程
void *func_yonghu(void *arg)
{
    pthread_detach(pthread_self());

    struct cfd_mysql cm;
    cm = *(struct cfd_mysql*)arg;
    char buf[BUFSIZ];
    char temp[100];
    char *query_str;
    char buff[BUFSIZ];
    int rows;
    MYSQL_RES *res;
    MYSQL_ROW row;

    //检索数据库中是否已为此用户建表
    //如果没表就以username新建一个表
    query_str = "show tables";
    rows = mysql_real_query(&cm.mysql, query_str, strlen(query_str));
    if(rows != 0)
        my_err("mysql_real_query", __LINE__);
    res = mysql_store_result(&cm.mysql);
    if(res == NULL)
        my_err("mysql_store_query", __LINE__);
    rows = mysql_num_rows(res);
    int count = 0;
    while(row = mysql_fetch_row(res))
    {
        count++;
        if(strcmp(cm.username, row[0]) == 0)
        {
            count--;
            break;
        }
    }
    if(count == rows)
        sprintf(temp, "create table %s(username varchar(20), num double, groupname varchar(20), groupnum double)", cm.username);


    sprintf(temp, "------%s------", cm.username);
    while(1)
    {
        Write(cm.cfd, temp);
        Write(cm.cfd, "[1] 好友列表\n[2] 添加好友\n[3] 群列表\n[4] 群选项\n[5] 退出登陆\n");
        read(cm.cfd, buf, sizeof(buf));
        if(strncmp(buf, "1\n", 2) == 0)
        {
            sprintf(buff, "select * from %s", cm.username);
            rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
            if(rows != 0)
                my_err("mysql_real_query error", __LINE__);
            res = mysql_store_result(&cm.mysql);
            if(res == NULL)
                my_err("mysql_store_result error", __LINE__);
            while(row = mysql_fetch_row(res))
            {
                if(atoi(row[5]) == 1)
                    printf("------%s---在线\n", row[0]);
                else
                    printf("------%s---离线\n", row[0]);
            }
            Write(cm.cfd, "---输入用户名，开始聊天\n");
            read(cm.cfd, buf, sizeof(buf));
            buf[strlen(buf) - 1] = '\0';
            mysql_repeat(&cm.mysql, cm.username, buf, 1);
        }
        else if(strncmp(buf, "2\n", 2) == 0)
        {
            
        }
        else if(strncmp(buf, "3\n", 2) == 0)
        {

        }
        else if(strncmp(buf, "4\n", 2) == 0)
        {

        }
        else if(strncmp(buf, "5\n", 2) == 0)
        {
            Write(cm.cfd, "[1] 登陆\n[2] 注册\n[3] 找回密码\n");
            break;
        }
        else
        {

        }
    }
    pthread_exit(0);
}