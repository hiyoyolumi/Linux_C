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

#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'

//欢迎界面信息
void welcome();
//用户界面信息
void welcome_1();
//好友界面信息-->好友列表
void welcome_friends();



//欢迎界面函数
void welcome_interface(int cfd);
//用户界面函数
void client_interface();
//好友界面函数
void friends_interface();



void Write(int fd, const char *buf);
void Read(int fd, void *buf, size_t count); //未有源码

//键入
int get_userinfo(char *buf, int len);

//把键入值发送给服务器，服务器与数据库中的数据进行比对，直到输入正确为止
void judge_userinfo(int fd, const char *string);