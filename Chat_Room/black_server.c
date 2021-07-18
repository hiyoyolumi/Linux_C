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

#define SERV_PORT 4507

void my_err(const char *str, const int line)
{
    fprintf(stderr, "%d : %s : %s", line, str, strerror(errno));
    exit(1);
}

int main()
{
    int serv_fd, conn_fd;
    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len;
    char buf[BUFSIZ];
    int i, ret;

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd == -1)
    {
        my_err("socket error", __LINE__);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htonl(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
    if (bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        my_err("bind err", __LINE__);
    }

    if (listen(serv_fd, 128) == -1)
    {
        my_err("listen error", __LINE__);
    }

    clit_addr_len = sizeof(clit_addr);
    conn_fd = accept(serv_fd, (struct sockaddr *)&clit_addr, &clit_addr_len);
    if (conn_fd == -1)
    {
        my_err("accept error", __LINE__);
    }

    while (1)
    {
        ret = recv(conn_fd, buf, sizeof(buf), 0);
        if (ret == -1)
        {
            my_err("recv error", __LINE__);
        }
        write(STDOUT_FILENO, buf, ret);

        for (i = 0; i < ret; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        if (send(conn_fd, buf, ret, 0) == -1)
        {
            my_err("send error", __LINE__);
        }
    }

    close(serv_fd);
    close(conn_fd);

    return 0;
}