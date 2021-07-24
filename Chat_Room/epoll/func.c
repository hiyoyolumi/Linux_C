#include "func.h"

extern int num_birth;
extern int clients[1000];
extern pthread_mutex_t mutex;

extern int lfd;
extern int epfd;
extern struct epoll_event tep, ep[MAXEVE];

extern int level;

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
    printf("[(1)] 登陆\n");
    printf("[(2)] 注册\n");
    printf("[(3)] 找回密码\n");
}

void welcome_1()
{
    printf("[a] 好友列表");
    printf("[b] 添加好友");
    printf("[c] 群列表");
    printf("[d] 群选项");
    printf("[e] 发送文件");
    printf("[f] 退出登陆");
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

int Read(int fd, char *buf, size_t count)
{
    if(read(fd, buf, sizeof(buf)) == -1)
    {
        my_err("read error", __LINE__);
    }
    int i=0;
    while(buf[i] != '\n')
        i++;
    i++;
    buf[i] = '\0';

    return i;
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
        // retval->retval = 1;
        Write(cm.cfd, "---请输入账号(q to quit):");
        n = read(cm.cfd, buf, sizeof(buf));
        retval->retval = huitui_val(buf);
        if(retval->retval == 0)
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
        if(retval->retval == 0)
            pthread_exit((void *)retval);
        buf[n - 1] = '\0';
        ret = mysql_repeat(&cm.mysql, "UserData", buf, 2);
        //printf("ret = %d\n", ret);
        if(ret == 0)
        {
            printf("[%s] ip %s input correct password\n", get_time(now_time), inet_ntoa(cm.clit_addr.sin_addr));
            Write(cm.cfd, "---正在登陆...\n");
            char temp[100];
            sprintf(temp, "update UserData set status = 1 where username = \"%s\"", retval->username);
            rows = mysql_real_query(&retval->mysql, temp, strlen(temp));
            if(rows != 0)
                my_err("mysql_real_query error", __LINE__);
            
            //登陆后将客户端的套接字在UserData表中更新
            // pthread_mutex_lock(&mutex);
            sprintf(temp, "update UserData set cfd = %d where username = \"%s\"", retval->cfd, retval->username);
            pthread_mutex_lock(&mutex);
            rows = mysql_real_query(&retval->mysql, temp, strlen(temp));
            if(rows != 0)
                my_err("mysql_real_query error", __LINE__);
            pthread_mutex_unlock(&mutex);

            // sprintf(temp, "update UserData set status = 1 where username = \"%s\"", retval->username);
            // pthread_mutex_lock(&mutex);
            // rows = mysql_real_query(&retval->mysql, temp, strlen(temp));
            // if(rows != 0)
            //     my_err("mysql_real_query error", __LINE__);
            // pthread_mutex_unlock(&mutex);
            // res = mysql_store_result(&retval->mysql);
            // if(res == NULL)
            //     my_err("mysql_store_result error", __LINE__);
            // while(row = mysql_fetch_row(res))
            // {
            //     //将该用户 设置为登陆状态
            //     if(strcmp(retval->username, row[0]) == 0)
            //     {
            //         sprintf(temp, "update UserData set status = %d where username = \"%s\"", 1, retval->username);
            //     }
            // }
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

    char temp[100];
    int rows;
    MYSQL_RES *res;
    MYSQL_ROW row;

    //检索数据库中是否已为此用户建表
    //如果没表就以username新建一个表
    strcpy(query_str, "show tables");
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
        if(strcmp(cm.username, row[0]) == 0)
        {
            count = 1;
            break;
        }
    }
    printf("count = %d\n", count);
    if(count == 0)
    {
        sprintf(temp, \
        "create table %s(friend varchar(20), type double)", \
                cm.username);
        rows = mysql_real_query(&cm.mysql, temp, strlen(temp));
        if(rows != 0)
            my_err("mysql_real_query error", __LINE__);
    }

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

            pthread_mutex_lock(&mutex);
            data.num = num_birth++;

            //向UserData中add数据
            sprintf(query_str, "\
insert into UserData values(\"%s\", \"%s\", \"%s\", \"%s\", \"%d\", \"%d\", \"%d\", \"0\")", \
data.username, data.password, data.nickname, data.mibao, data.num, 0, cm.cfd);
            pthread_mutex_unlock(&mutex);

            //printf("[%s] %s", get_time(now_time), query_str);
            mysql_add(&cm.mysql, query_str, cm.clit_addr, "UserData");
            printf("[%s] %s", get_time(now_time), query_str);
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
    int rows;
    MYSQL_RES *res;
    MYSQL_ROW row;

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
                    rows = mysql_real_query(&cm.mysql, query_str, strlen(query_str));
                    if(rows != 0)
                        my_err("mysql_real_query error", __LINE__);
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
    return 1;
}

int huitui(const char *buf)
{
    if(strncmp(buf, "q\n", 2) == 0) 
    {
        printf("cancel\n");
        pthread_exit((void *)0);
    }
}


//分离状态的线程---用户界面
void *func_yonghu(void *arg)
{
    pthread_detach(pthread_self());

    int newsnum;
    char now_time[100];
    struct cfd_mysql cm;
    cm = *(struct cfd_mysql*)arg;
    char buf[BUFSIZ];
    char temp[100];
    char *query_str;
    char buff[BUFSIZ];
    int rows;
    MYSQL_RES *res, *res2;
    MYSQL_ROW row, row2;
    // //检索数据库中是否已为此用户建表
    // //如果没表就以username新建一个表
    // query_str = "show tables";
    // rows = mysql_real_query(&cm.mysql, query_str, strlen(query_str));
    // if(rows != 0)
    //     my_err("mysql_real_query", __LINE__);
    // res = mysql_store_result(&cm.mysql);
    // if(res == NULL)
    //     my_err("mysql_store_query", __LINE__);
    // rows = mysql_num_rows(res);
    // int count = 0;
    // while(row = mysql_fetch_row(res))
    // {
    //     if(strcmp(cm.username, row[0]) == 0)
    //     {
    //         count = 1;
    //         break;
    //     }
    // }
    // printf("count = %d\n", count);
    // if(count == 0)
    // {
    //     sprintf(temp, \
    //     "create table %s(friend varchar(20), type double)", \
    //             cm.username);
    //     rows = mysql_real_query(&cm.mysql, temp, strlen(temp));
    //     if(rows != 0)
    //         my_err("mysql_real_query error", __LINE__);
    // }
    
    int stat[200];
    int sta;
    int ret;
    pthread_t thid;
    query_str = "select * from UserData";
    sprintf(temp, "------%s------\n", cm.username);
    while(1)
    {
        int i = 0;
        int j;
        Write(cm.cfd, temp);
        sprintf(buff, "select * from UserData where username = \"%s\"", cm.username);
        rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
        if(rows != 0)
            my_err("mysql_real_query error", __LINE__);
        res = mysql_store_result(&cm.mysql);
        if(res == NULL)
            my_err("mysql_store_result error", __LINE__);
        while(row = mysql_fetch_row(res))
        {
            sprintf(buff, "------news(%d)------(v to view)\n", atoi(row[7]));
            newsnum = atoi(row[7]);
            Write(cm.cfd, buff);
            break;
        }
        Write(cm.cfd, "[a] 好友列表\n[b] 添加好友\n[c] 群列表\n[d] 群选项\n[e] 退出登陆\n");
        read(cm.cfd, buf, sizeof(buf));
        if(strncmp(buf, "v\n", 2) == 0)
        {
            while(1)
            {
                Write(cm.cfd, "输入序号以处理消息(q to quit)\n");
                //读取OffLineMes中发给自己的消息
                // query_str = "select * from OffLineMes";
                sprintf(buff, "select * from OffLineMes where touser = \"%s\"", cm.username);
                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                if(rows != 0)
                    my_err("mysql_real_query error", __LINE__);
                res = mysql_store_result(&cm.mysql);
                if(res == NULL)
                    my_err("mysql_store_result error", __LINE__);
                while(row = mysql_fetch_row(res))
                {
                    sprintf(buff, "[%d]-<%s>-(%s)---%s\n", i++, row[0], row[1], row[3]);
                    Write(cm.cfd, buff);
                }
                read(cm.cfd, buf, sizeof(buf));
                if(strncmp(buf, "q\n", 2) == 0)
                    break;
                // buf[strlen(buf) - 1] = '\0';
                for(j=0; j<i; j++)
                {
                    if(atoi(buf) == j)
                    {
                        break;
                    }
                }
                sprintf(buff, "select * from OffLineMes where touser = \"%s\"", cm.username);
                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                if(rows != 0)
                    my_err("mysql_real_query error", __LINE__);
                res = mysql_store_result(&cm.mysql);
                if(res == NULL)
                    my_err("mysql_store_result error", __LINE__);
                i = 0;
                while(row = mysql_fetch_row(res))
                {
                    if(j == i++)    //从OffLineMes中找到自己要处理的消息
                    {
                        if(atoi(row[4]) == 1)   //加好友类型的消息
                        {
                            Write(cm.cfd, "---同意(t)/拒绝(f)---\n");
                            read(cm.cfd, buf, sizeof(buf));
                            if(strncmp(buf, "q\n", 2) == 0)
                                break;
                            //buf[strlen(buf) - 1] = '\0';
                            if(strncmp(buf, "t\n", 2) == 0)   //同意加好友申请
                            {   
                                //给自己一个回馈
                                Write(cm.cfd, "已接受\n");
                                //给加好友的人一个回馈
                                sprintf(buff, "insert into OffLineMes values\
                                (\"%s\", \"%s\", \"%s\", \"对方接受了你的好友申请\", \"%d\")", \
                                get_time(now_time), row[2], row[1], 2);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //给加好友的人的newsnum + 1
                                newsnum = mysql_inquire_newsnum(&cm.mysql, row[1], __LINE__);
                                sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        ++newsnum,                row[1]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //把好友加到自己的好友列表中
                                sprintf(buff, "insert into %s values(\"%s\", \"1\")", cm.username, row[1]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //把自己加到对方的好友列表中
                                sprintf(buff, "insert into %s values(\"%s\", \"1\")", row[1], row[2]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //获取自己当前的newsnum，并把它更新到自己的信息中
                                newsnum = mysql_inquire_newsnum(&cm.mysql, row[2], __LINE__);
                                sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        --newsnum,                row[2]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_create error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //把这条加好友消息从OffLineMes表中抹去
                                sprintf(buff, "delete from OffLineMes where time = \"%s\"", \
                                                                                    row[0]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        printf("error:%s\n", mysql_error(&cm.mysql));
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);
                            }
                            else if(strncmp(buf, "f\n", 2) == 0)  //拒绝加好友申请
                            {
                                //给自己一个回馈
                                Write(cm.cfd, "已拒绝\n");
                                //给加好友的人一个回馈
                                sprintf(buff, "insert into OffLineMes values\
                                (\"%s\", \"%s\", \"%s\", \"对方拒绝了你的好友申请\", \"%d\")", \
                                get_time(now_time), row[2], row[1], 2);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //获取自己的未读信息，把自己的未读信息数量newsnum更新
                                newsnum = mysql_inquire_newsnum(&cm.mysql, row[2], __LINE__);
                                sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        --newsnum,                row[2]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_create error", __LINE__);
                                pthread_mutex_unlock(&mutex);
                                
                                //获取加好友的人的newsnum，并将其更新
                                newsnum = mysql_inquire_newsnum(&cm.mysql, row[1], __LINE__);
                                sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                            ++newsnum,            row[1]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);

                                //把这条加好友消息从OffLineMes中抹去
                                sprintf(buff, "delete from OffLineMes where time = \"%s\"", \
                                                                                    row[0]);
                                pthread_mutex_lock(&mutex);
                                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                    if(rows != 0)
                                        my_err("mysql_real_query error", __LINE__);
                                pthread_mutex_unlock(&mutex);
                            }
                        }
                        else if(atoi(row[4]) == 0)   //聊天型消息,可进入聊天框
                        {
                            //获取当前自己的未读消息newsnum，并将其更新到UserData中
                            newsnum = mysql_inquire_newsnum(&cm.mysql, row[2], __LINE__);
                            sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        --newsnum,           row[2]);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_create error", __LINE__);
                            pthread_mutex_unlock(&mutex);

                            //把这条未读消息从未读消息OffLineMes中抹去
                            sprintf(buff, "delete from OffLineMes where time = \"%s\"", \
                                                                                row[0]);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);

                            sprintf(buff, "select status from UserData where username = \"%s\"", row[1]);
                            rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                            if(rows != 0)
                                my_err("mysql_real_query error", __LINE__);
                            res = mysql_store_result(&cm.mysql);
                            if(res == NULL)
                                my_err("mysql_store_result error", __LINE__);
                            while(row2 = mysql_fetch_row(res))
                                sta = atoi(row2[5]);
                            if(sta == 1)
                            {
                                //进入聊天界面,对方回复消息需要手动进入与自己的聊天框中
                                if(pthread_create(&thid, NULL, func_liaotian, (void *)&cm) == 0)
                                    my_err("pthread_create error", __LINE__);
                                pthread_join(thid, NULL);
                            }
                            else if(sta == 0)
                            {
                                sprintf(buff, "------%s(离线)(q to quit)------", row[1]);
                                Write(cm.cfd, buff);
                                while(1)
                                {
                                    read(cm.cfd, buf, sizeof(buf));
                                    if(strncmp(buf, "q\n", 2) == 0)
                                        break;
                                    buf[strlen(buf) - 1] = '\0';

                                    //把消息发送到未读消息OffLineMes中
                                    sprintf(buff, "insert into OffLineMes values\
                                    (\"%s\", \"%s\", \"%s\", \"%s\", \"0\")", \
                                    get_time(now_time), row[2], row[1], buf);
                                    pthread_mutex_lock(&mutex);
                                        rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                        if(rows != 0)
                                            my_err("mysql_real_query error", __LINE__);
                                    pthread_mutex_unlock(&mutex);

                                    //获取对方当前的newsnum，并更新值
                                    newsnum = mysql_inquire_newsnum(&cm.mysql, row[1], __LINE__);
                                    sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                                ++newsnum,          row[1]);
                                    pthread_mutex_lock(&mutex);
                                        rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                        if(rows != 0)
                                            my_err("mysql_real_query error", __LINE__);
                                    pthread_mutex_unlock(&mutex);
                                }
                            }
                        }
                        else if(atoi(row[4]) == 2)   //只读型消息
                        {
                            //看过之后将自己的未读消息newsnum更新
                            newsnum = mysql_inquire_newsnum(&cm.mysql, row[2], __LINE__);
                            sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        --newsnum,                row[2]);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_create error", __LINE__);
                            pthread_mutex_unlock(&mutex);

                            //将这条消息从位读消息OffLineMes中抹去
                            sprintf(buff, "delete from OffLineMes where time = \"%s\"", \
                                                                                row[0]);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);
                        }
                        break;
                    }
                    
                }
                if(strncmp(buf, "q\n", 2) == 0)
                    break;
            }
        }
        else if(strncmp(buf, "a\n", 2) == 0)
        {
            while(1)
            {
                // int k = 0;
                sprintf(buff, "select * from %s", cm.username);
                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                if(rows != 0)
                    my_err("mysql_real_query error", __LINE__);
                res = mysql_store_result(&cm.mysql);
                if(res == NULL)
                    my_err("mysql_store_result error", __LINE__);
                while(row = mysql_fetch_row(res))
                {   
                    sprintf(buff, "select * from UserData where username = \"%s\"", row[0]);
                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                    if(rows != 0)
                    {
                        printf("error:%s\n", mysql_error(&cm.mysql));
                        my_err("mysql_real_query error", __LINE__);
                    }
                    res2 = mysql_store_result(&cm.mysql);
                    if(atoi(row2[5]) == 1)
                    {
                        //sta = 1;
                        printf("------%s---在线\n", row[0]);
                        // stat[k++] = 1;
                    }
                    else if(atoi(row2[5]) == 0)
                    {
                        //sta = 0;
                        printf("------%s---离线\n", row[0]);
                        // stat[k++] = 0;
                    }

                    // rows = mysql_real_query(&cm.mysql, "select * from UserData", strlen("select * from UserData"));
                    // if(rows != 0)
                    //     my_err("mysql_real_query error", __LINE__);
                    // res2 = mysql_store_result(&cm.mysql);
                    // if(res2 == NULL)
                    //     my_err("mysql_real_query error", __LINE__);
                    // while(row2 = mysql_fetch_row(res2))
                    // {
                    //     if(strcmp(row2[0], row[0]) == 0)
                    //     {
                    //         if(atoi(row2[5]) == 1)
                    //         {
                    //             //sta = 1;
                    //             printf("------%s---在线\n", row[0]);
                    //             stat[k++] = 1;
                    //         }
                    //         else if(atoi(row2[5]) == 0)
                    //         {
                    //             //sta = 0;
                    //             printf("------%s---离线\n", row[0]);
                    //             stat[k++] = 0;
                    //         }
                    //     }
                    // }
                    // int k = 0;
                    // if(sta == 1)
                    // {
                    //     printf("------%s---在线\n", row[0]);
                    //     stat[k++] = 1;
                    // }
                    // else if(sta == 0)
                    // {
                    //     printf("------%s---离线\n", row[0]);
                    //     stat[k++] = 0;
                    // }
                    //if(atoi(row[5]) == 1)
                        // printf("------%s\n", row[0]);
                    //else
                        //printf("------%s---离线\n", row[0]);
                }
                //进入聊天
                Write(cm.cfd, "---输入用户名，开始聊天(q to quit)\n");
                read(cm.cfd, buf, sizeof(buf));
                if(strncmp(buf, "q\n", 2) == 0)
                {
                    break;
                }
                buf[strlen(buf) - 1] = '\0';
                ret = mysql_repeat(&cm.mysql, cm.username, buf, 1);
                if(ret == 0)
                {
                    sprintf(buff, "select status from UserData where username = \"%s\"", buf);
                    rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                    if(rows != 0)
                        my_err("mysql_real_query error", __LINE__);
                    res = mysql_store_result(&cm.mysql);
                    if(res == NULL)
                        my_err("mysql_store_result error", __LINE__);
                    while(row = mysql_fetch_row(res))
                        sta = atoi(row[0]);
                    if(sta == 1)    //在线状态，直接进入聊天界面，发消息给cm.tousername
                    {
                        strcpy(cm.tousername, buf);
                        if(pthread_create(&thid, NULL, func_liaotian, (void *)&cm) == -1)
                            my_err("pthread_create error", __LINE__);
                        pthread_join(thid, NULL);
                        break;
                    }
                    else if(sta == 0)   //离线状态，进入聊天界面，发消息给OffLineMes
                    {
                        strcpy(cm.tousername, buf);
                        sprintf(buff, "------%s(离线)(q to quit)------\n", cm.tousername);
                        Write(cm.cfd, buff);
                        while(1)
                        {
                            read(cm.cfd, buf, sizeof(buf));
                            if(strncmp(buf, "q\n", 2) == 0)
                                break;

                            //把消息发送到未读消息OffLineMes中
                            sprintf(buff, "insert into OffLineMes values\
                            (\"%s\", \"%s\", \"%s\", \"%s\", \"0\")", \
                            get_time(now_time), cm.username, cm.tousername, buf);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);

                            
                            // sprintf(buff, "select newsnum from UserData where username = \"%s\"", \
                            //                                                             cm.username);
                            // rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                            // if(rows != 0)
                            //     my_err("mysql_real_query error", __LINE__);
                            // res = mysql_store_result(&cm.mysql);
                            // while(row = mysql_fetch_row(res))
                            //     newsnum = atoi(row[0]);

                            //获取自己当前的newsnum，并把它更新到对方的未读数newsnum中
                            newsnum = mysql_inquire_newsnum(&cm.mysql, cm.tousername, __LINE__);
                            sprintf(buff, "update UserData set newsnum = %d where username = \"%s\"", \
                                                                        ++newsnum,          cm.username);
                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, buff, strlen(buff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);
                        }
                    }
                }
                else
                {
                    Write(cm.cfd, "---你的好友列表中没有此用户\n");
                    continue;
                }
            }
        }
        else if(strncmp(buf, "b\n", 2) == 0)
        {
            while(1)
            {
                int flag = 0;
                Write(cm.cfd, "------添加好友(1)/群(2)(q to quit)------\n");
                read(cm.cfd, buf, sizeof(buf));
                if(strncmp(buf, "q\n", 2) == 0)
                    break;
                else if(strncmp(buf, "1\n", 2) == 0)
                {
                    Write(cm.cfd, "---请输入用户名(q to quit):");
                    read(cm.cfd, buf, sizeof(buf));
                    if(strncmp(buf, "q\n", 2) == 0)
                        break;
                    buf[strlen(buf) - 1] = '\0';
                    //strcpy(buff, buf);
                    query_str = "select * from UserData";
                    rows = mysql_real_query(&cm.mysql, query_str, strlen(query_str));
                    if(rows != 0)
                        my_err("mysql_real_query", __LINE__);
                    res = mysql_store_result(&cm.mysql);
                    if(res == NULL)
                        my_err("mysql_store_result error", __LINE__);
                    while(row = mysql_fetch_row(res))
                    {
                        if(strcmp(buf, row[0]) == 0)
                        {
                            flag = 1;
                            Write(cm.cfd, "已向对方发送请求，等待对方接受\n");
                            strcpy(buff, "请求添加你为好友\n");
                            strcpy(cm.tousername, row[0]);
                            char duff[BUFSIZ];
                            sprintf(duff, \
                                    "insert into OffLineMes values(\"%s\", \"%s\", \"%s\", \"%s\", \"1\")",\
                                    get_time(now_time), cm.username, cm.tousername, buff);

                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, duff, strlen(duff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);

                            newsnum = atoi(row[7]);
                            newsnum++;
                            sprintf(duff, \
                            "update UserData set newsnum = %d where username = \"%s\"", \
                            newsnum, row[0]);

                            pthread_mutex_lock(&mutex);
                                rows = mysql_real_query(&cm.mysql, duff, strlen(duff));
                                if(rows != 0)
                                    my_err("mysql_real_query error", __LINE__);
                            pthread_mutex_unlock(&mutex);
                        }
                    }
                    if(flag == 0)
                        Write(cm.cfd, "没有此用户\n");
                }
                else if(strncmp(buf, "2\n", 2) == 0)
                {
                    Write(cm.cfd, "---请输入群聊名(q to quit):");
                    read(cm.cfd, buf, sizeof(buf));
                    if(strncmp(buf, "q\n", 2) == 0)
                        break;
                    buf[strlen(buf) - 1] = '\0';
                    strcpy(buff, buf);


                }
            }
        }
        else if(strncmp(buf, "c\n", 2) == 0)
        {

        }
        else if(strncmp(buf, "d\n", 2) == 0)
        {

        }
        else if(strncmp(buf, "e\n", 2) == 0)
        {
            Write(cm.cfd, "[(1)] 登陆\n[(2)] 注册\n[(3)] 找回密码\n");
            break;
        }
        else
        {

        }
    }
    pthread_exit(0);
}

void *func_liaotian(void *arg)
{
    struct cfd_mysql cm;
    cm = *(struct cfd_mysql *)arg;
    int i = 0;
    int in = 0, out = 0;     //下标位置
    int incfd, outcfd;
    char temp[100];
    char buf[BUFSIZ];
    char now_time[100];

    int rows;
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(temp, "select * from UserData where username = \"%s\" or username = \"%s\"", cm.username, cm.tousername);
    rows = mysql_real_query(&cm.mysql, temp, strlen(temp));
    if(rows != 0)
        my_err("mysql_real_query error", __LINE__);
    res = mysql_store_result(&cm.mysql);
    if(res == NULL)
        my_err("mysql_store_result error", __LINE__);
    while(row = mysql_fetch_row(res))
    {
        if(strcmp(cm.username, row[0]) == 0)
        {    
            in = atoi(row[4]);
            incfd = atoi(row[6]);
        }
        else if(strcmp(cm.tousername, row[0]) == 0)
        {
            out = atoi(row[4]);
            outcfd = atoi(row[6]);
        }
    }

    // pthread_mutex_lock(&mutex);
    // clients[in] = outcfd;
    // clients[out] = incfd;
    // pthread_mutex_unlock(&mutex);

    sprintf(temp, "------%s------(q to quit)\n", cm.tousername);
    Write(cm.cfd, temp);
    //Write(cm.cfd, "(q to quit)\n");
    read(cm.cfd, buf, sizeof(buf));
    if(strncmp(buf, "q\n", 2) == 0)
    {
        pthread_exit(0);
    }
    int ret;
    char infor[BUFSIZ];    
    // sprintf(infor, "[%s]-<%s>---%s", get_time(now_time), cm.username, buf);
    // Write(outcfd, infor);
    while(1)
    {
        ret = epoll_wait(epfd, ep, MAXEVE, -1);
        for(i=0; i<ret; i++)
        {
            if(ep[i].data.fd == lfd)
            {
                continue;
            }
            else
            {
                //读套接字
                if(incfd == ep[i].data.fd)
                {
                    read(incfd, buf, sizeof(buf));
                    if(strncmp(buf, "q\n", 2) == 0)
                        pthread_exit(0);
                    sprintf(infor, "[%s]-<%s>---%s", get_time(now_time), cm.username, buf);
                    Write(outcfd, infor);
                }
                else if(outcfd == ep[i].data.fd)
                {
                    read(outcfd, buf, sizeof(buf));
                    if(strncmp(buf, "q\n", 2) == 0)
                        pthread_exit(0);
                    sprintf(infor, "[%s]-<%s>---%s", get_time(now_time), cm.tousername, buf);
                    Write(incfd, infor);
                }
            }
        }
    }
}