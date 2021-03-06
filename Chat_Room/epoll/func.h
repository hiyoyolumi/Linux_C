#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include "time.h"

#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'

#define MAXEVE 1024

struct node
{
    char username[20];
    char password[20];
    char nickname[20];
    char mibao[20];
    int num;
};

struct cfd_mysql
{
    int cfd;
    MYSQL mysql;
    struct sockaddr_in clit_addr;
    char username[20];
    char tousername[20];
    int tocfd;
    int retval;
};

struct name_fd_sql
{
    char username[20];
    struct cfd_mysql cm;
};

//
//打印类函数
//
//欢迎界面信息
void welcome();
//用户界面信息
void welcome_1();
//好友界面信息-->好友列表
void welcome_friends();
//获取系统当前时间
char *get_time();

//
//引导类函数
//
//欢迎界面函数
void welcome_interface(int cfd);
//用户界面函数
void client_interface();
//好友界面函数
void friends_interface();


//
//错误处理类函数
//
void Write(int fd, const char *buf);
//简单错误处理
int Read(int fd, char *buf, size_t count);


//
//实用类函数
//
//键入
int get_userinfo(char *buf, int len);

//把键入值发送给服务器，服务器与数据库中的数据进行比对，直到输入正确为止
void judge_userinfo(int fd, const char *string);

//解析客户端发来的请求,并把请求转化为整数
int parse_request(char *request);


void my_err(const char *str, const int line);


//
//mysql类函数
//
//向表中add数据
//str为insert语句
void mysql_add(MYSQL *mysql, const char *str, const struct sockaddr_in clit_addr, const char *table);

//建表
//str为建表语句
void mysql_build(MYSQL *mysql, const char *str);

//查表（查询str是否重复）
//string是表名
//str是待查询的字符串
//field是第几列
//有重复返回0,无重复返回1
int mysql_repeat(MYSQL *mysql, const char *string, const char *str, int field);

//查询newsnum
//username是待查询的用户名
int mysql_inquire_newsnum(MYSQL *mysql, const char *username, int line);

//server多线程函数
int huitui(const char *buf);
//输入q的话返回0
//其它返回1
int huitui_val(const char *buf);

void *func_zhuce(void *arg);
void *func_denglu(void *arg);
void *func_zhaohui(void *arg);
void *func_yonghu(void *arg);
void *func_liaotian(void *arg);