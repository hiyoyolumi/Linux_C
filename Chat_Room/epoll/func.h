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

#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'


struct node
{
    char account[20];       //账号
    char password[20];      //密码
    char name[20];          //昵称
    char security[20];      //密保
    int serial;             //服务器标识序号(从0开始)，可用于client的下标
};

struct cfd_mysql
{
    int cfd;
    MYSQL mysql;
    struct sockaddr_in clit_addr;
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
void Read(int fd, void *buf, size_t count); //未有源码


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


//server多线程函数
void *func_zhuce(void *arg);