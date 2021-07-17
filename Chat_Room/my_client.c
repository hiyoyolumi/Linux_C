#include "my_server.h"

#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'

int get_userinfo(char *buf, int len)
{
    if(buf == NULL)
    {
        return -1;
    }

    int i = 0;
    char c;
    while(((c = getchar()) != '\n') && (c != EOF) && (i < len - 2))
    {
        buf[i++] = c;
    }
    buf[i++] = '\n';
    buf[i++] = '\0';

    return 0;
}

void input_userinfo(int conn_fd, const char * string)
{
    char input_buf[32];
    char recv_buf[BUFSIZE];
    int flag_userinfo;

    do
    {
        printf("%s:", string);
        if(get_userinfo(input_buf, 32) == -1)
        {
            printf("getuserinfo error: %s\n", strerror(errno));
            exit(1);
        }
        if(send(conn_fd, input_buf, strlen(input_buf), 0) == -1)
        {
            printf("send error: %s\n", strerror(errno));
            exit(1);
        }
        if(my_recv(conn_fd, recv_buf, sizeof(recv_buf)) == -1)
        {
            printf("my_recv error: %s\n", strerror(errno));
            exit(1);
        }
        if(recv_buf[0] == VALID_USERINFO)
        {
            flag_userinfo = VALID_USERINFO;
        }
        else
        {
            printf("%s wrong, input again\n");
            flag_userinfo = INVALID_USERINFO;
        }
    } while (flag_userinfo == INVALID_USERINFO);
}

int main(int argc, char ** argv)
{
    int ret;
    int conn_fd;
    int serv_port;
    struct sockaddr_in serv_addr;
    char recv_buf[BUFSIZE];

    if(argc != 5)
    {
        printf("usage: [-p] [serv_port] [-a] [serv_address]\n");
        exit(1);
    }

    memset(serv_addr.sin_zero, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    for(int i=1; i<argc; i++)
    {
        if(strcmp("-p", argv[i]) == 0)
        {
            serv_port = atoi(argv[i+1]);
            if(serv_port < 0 || serv_port >65535)
            {
                printf("invalid serv_addr.sin_port\n");
                exit(1);
            }
            else
            {
                serv_addr.sin_port = htons(serv_port);
            }
            continue;
        }
        if(strcmp("-a", argv[i]) == 0)
        {
            if(inet_aton(argv[i+1], &serv_addr.sin_addr) == 0)
            {
                printf("inet_aton error: %s\n", strerror(errno));
                exit(1);
            }
            continue;
        }
    }
    if(serv_port == 0 || serv_addr.sin_addr.s_addr == 0)
    {
        printf("usage: [-p] [serv_addr.sin_port] [-a] [serv_addr.sin_addr]\n");
        exit(1);
    }

    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(conn_fd == -1)
    {
        printf("socket error: %s\n", strerror(errno));
    }

    if(connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("connect error: %s\n", strerror(errno));
        exit(1);
    }

    input_userinfo(conn_fd, "username");
    input_userinfo(conn_fd, "password");

    if((ret = my_recv(conn_fd, recv_buf, sizeof(recv_buf))) < 0)
    {
        printf("data is too long\n");
        exit(1);
    }
    for(int i=0; i<ret; i++)
    {
        printf("%c", recv_buf[i]);
    }
    printf("\n");

    close(conn_fd);

    return 0;
}