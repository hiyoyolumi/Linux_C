#include "func.h"

extern pthread_mutex_t mutex;
// void mysql_build(MYSQL *mysql, const char *str)
// {
//     int rc;

//     rc = mysql_real_query(mysql, str, strlen(str));
//     if(rc != 0)
//         my_err("mysql_real_query error", __LINE__);
// }

void mysql_add(MYSQL *mysql, const char *str, const struct sockaddr_in clit_addr, const char *table)
{
    int i;
    int rc;
    int rows, fields;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char str_select[20];

    sprintf(str_select, "select * from %s", table);

    rc = mysql_real_query(mysql, str, strlen(str));
    if(rc != 0)
        my_err("mysql_real_query error", __LINE__);
    

    rc = mysql_real_query(mysql, str_select, strlen(str_select));
    if(rc != 0)
        my_err("mysql_real_query error", __LINE__);
    res = mysql_store_result(mysql);
    if(res == NULL)
        my_err("mysql_store_result error", __LINE__);

    rows = mysql_num_rows(res);
    fields = mysql_num_fields(res);

    //在服务器中 打印出客户的注册信息
    printf("recive news from ip %s\n", inet_ntoa(clit_addr.sin_addr));
    while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }
}

int mysql_repeat(MYSQL *mysql, const char *string, const char *str, int field)
{
    char buf[100];
    int fields;
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(buf, "select * from %s", string);
    mysql_real_query(mysql, buf, strlen(buf));
    res = mysql_store_result(mysql);
    fields = mysql_num_fields(res);
    if(field > fields)
    {
        printf("field out of range\n");
    }
    while((row = mysql_fetch_row(res)))
    {
        if(strcmp(row[field - 1], str) == 0)
        {
            return 0;   //重复
        }
    }
    return 1;           //无重复
}

int mysql_inquire_newsnum(MYSQL *mysql, const char *username, int line)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    int rows;
    char buff[BUFSIZ];

    sprintf(buff, "select newsnum from UserData where username = \"%s\"", username);
    pthread_mutex_lock(&mutex);
        rows = mysql_real_query(mysql, buff, strlen(buff));
        if(rows != 0)
            my_err("mysql_real_query error", line);
        res = mysql_store_result(mysql);
        if(res == NULL)
            my_err("mysql_store_result error", line);
    pthread_mutex_unlock(&mutex);
    while(row = mysql_fetch_row(res))
        return atoi(row[0]);
}