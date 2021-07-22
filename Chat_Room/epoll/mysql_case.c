#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>


int main(int argc, const char *argv[])
{
    MYSQL           mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    char            *query_str = NULL;
    int             rc, i, fields;
    int             rows;
    if (NULL == mysql_init(&mysql)) {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
    if (NULL == mysql_real_connect(&mysql,
                "127.0.0.1",
                "root",
                "xjmwsb1234",
                "testdb",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("1. Connected MySQL successful! \n");

    query_str = "select * from UserData";
    rows = mysql_real_query(&mysql, query_str, strlen(query_str));
    res = mysql_store_result(&mysql);
    fields = mysql_num_fields(res);
    int count = 0;
    while(row = mysql_fetch_row(res))
    {
        printf("sizeof = %d\n", sizeof(row[4]));
    }
    //printf("count = %d\n", count);




    // //1.操作前先给query_str赋值
    // query_str = "insert into 学生数据 values(\"小明\",\"三年级一班\",\"00001\",100,50)";
    // mysql_real_query(&mysql, query_str, strlen(query_str));

    // query_str = "select * from 学生数据";
    // rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    // if (0 != rc) {
    //     printf("mysql_real_query(): %s\n", mysql_error(&mysql));
    //     return -1;
    // }
    // //2.获取行和列
    // res = mysql_store_result(&mysql);
    // if (NULL == res) {
    //      printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
    //      return -1;
    // }
    // rows = mysql_num_rows(res);
    // printf("The total rows is: %d\n", rows);
    // fields = mysql_num_fields(res);
    // printf("The total fields is: %d\n", fields);
    // //3.打印出整个表
    // while ((row = mysql_fetch_row(res))) {
    //     for (i = 0; i < fields; i++) {
    //         printf("%s\t", row[i]);
    //     }
    //     printf("\n");
    // }

    // query_str = "create table UserData(account varchar(20), password varchar(20), nickname varchar(20), security varchar(20), number double)";
    // mysql_real_query(&mysql, query_str, strlen(query_str));

    // query_str = "insert into UserData values(\"abc\", \"123456\", \"ztd\", \"20030111\", \"0\")";
    // mysql_real_query(&mysql, query_str, strlen(query_str));

    // query_str = "select * from UserData";
    // mysql_real_query(&mysql, query_str, strlen(query_str));

    // res = mysql_store_result(&mysql);
    // rows = mysql_num_rows(res);
    // printf("The total rows is: %d\n", rows);
    // fields = mysql_num_fields(res);
    // printf("The total fields is: %d\n", fields);

    // while ((row = mysql_fetch_row(res))) {
    //     for (i = 0; i < fields; i++) {
    //         printf("%s\t", row[i]);
    //     }
    //     printf("\n");
    // }

    //query_str = "delete from UserData where account = \"abc\"";
    //mysql_real_query(&mysql, query_str, strlen(query_str));

    mysql_close(&mysql);
    return 0;
}