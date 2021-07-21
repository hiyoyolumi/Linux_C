#include "func.h"

// void welcome_interface(int cfd)
// {
//     int a;
//     int buf[20];

//     //打印软件欢迎界面信息
//     welcome();
//     //输入选项
//     scanf("%d", &a);
//     switch(a)
//     {
//         case 1:     //登陆
//             //输入用户名
//             judge_userinfo(cfd, "username");
//             //输入用户密码
//             judge_userinfo(cfd, "password");
//             printf("login success!\n");
//             //进入用户界面
//             client_interface();

//         case 2:     //注册
//             //向服务器发送注册信息
//             strcpy(buf, "zhuce");
//             write(cfd, buf, strlen(buf));
//             //输入用户名
//             scanf("%s", buf);
//                 //比对用户名是否重复
//                 write(cfd, buf, strlen(buf));

//             //输入用户密码

//         case 3:     //找回密码
//             //输入用户名

//             //输入新密码


//     }
// }

// void client_interface()
// {
//     int a;

//     //打印用户界面信息
//     welcome_1();
//     scanf("%d", &a);
//     switch(a)
//     {
//         case 1:     //好友列表
//             //打印出自己的好友 及选项
//             friends_interface();

//         case 2:     //添加好友
//             //输入要添加的好友的账号，发送给服务器

//         case 3:     //群列表
//             //打印出自己加入的群

//         case 4:     //群选项
//             //打印出群界面
        
//         case 5:     //发送文件
//             //未知
//     }
// }

// void friends_interface()
// {
//     //打印好友列表
//     welcome_friends();
// }
