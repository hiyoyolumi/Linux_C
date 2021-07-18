#include "my_server.h"

#define SERV_PORT 4507
#define LISTENQ 12

#define USERNAME 0
#define PASSWORD 1

struct userinfo
{
    char username[32];
    char password[32];
};

struct userinfo users[] = 
{
    {"linux", "unix"},
    {"4507", "4508"},
    {"clh", "clh"},
    {"xl", "xl"},
    {" ", " "}
};

int find_name(const char *name)
{
    if(name == NULL)
    {
        printf("in find name, NULL pointer\n");
        return -2;
    }
    for(int i=0; users[i].username[0] != ' '; i++)
    {
        if(strcmp(name, users[i].username) == 0)
        {
            return i;
        }
    }

    return -1;
}

void send_data(int conn_fd, const char * string)
{
    if(send(conn_fd, string, strlen(string), 0) == -1)
    {
        printf("send error: %s", strerror(errno));
        exit(1);
    }
}

int main()
{
    int sock_fd, conn_fd;
    int optval;
    int flag_recv = USERNAME;
    int ret;
    int name_num;
    pid_t pid;
    socklen_t cli_len;
    struct sockaddr_in cli_addr, serv_addr;
    char recv_buf[128];
    //socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        printf("socket error: %s\n", strerror(errno));
        exit(1);
    }

    optval = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(optval)) == -1)
    {
        printf("setsockopt error: %s\n", strerror(errno));
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serv_addr.sin_zero, 0, sizeof(struct sockaddr_in));
    //bind
    if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("bind error: %s\n", strerror(errno));
        exit(1);
    }
    //listen
    if(listen(sock_fd, LISTENQ) == -1)
    {
        printf("listen errno: %s\n", strerror(errno));
        exit(1);
    }

    cli_len = sizeof(struct sockaddr_in);
    while(1)
    {
        //accept
        conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if(conn_fd < 0)
        {
            printf("accept errno: %s\n", strerror(errno));
            exit(1);
        }
        printf("accept a new client, IP:%s\n", inet_ntoa(cli_addr.sin_addr));

        pid = fork();
        if(pid == 0)
        {
            close(sock_fd);     //关闭用于与建立连接的套接字
            while(1)
            {
                ret = recv(conn_fd, recv_buf, BUFSIZE, 0);
                if(ret == -1)
                {
                    printf("recv error:%s\n", strerror(errno));
                    exit(1);
                }
                recv_buf[ret - 1] = '\0';
                
                if(flag_recv == USERNAME)
                {
                    name_num = find_name(recv_buf);
                    switch (name_num)
                    {
                        case -1:
                            send_data(conn_fd, "n\n");
                            break;
                        case -2:
                            printf("find_name() error: %s\n", strerror(errno));
                            exit(1);
                            break;
                        default:
                            send_data(conn_fd, "y\n");
                            flag_recv = PASSWORD;
                            break;
                    }
                }
                else if(flag_recv == PASSWORD)
                {
                    if(strcmp(recv_buf, users[name_num].password) == 0)
                    {
                        send_data(conn_fd, "y\n");
                        send_data(conn_fd, "welcome to my server\n");
                        printf("%s login\n", users[name_num].username);
                        break;  //跳出while循环
                    }
                }
                else
                {
                    send_data(conn_fd, "n\n");
                }
            }
        }
        else    //父进程
        {
            close(conn_fd); //关闭用于与客户端通信的套接字
        }
    }

    return 0;
}