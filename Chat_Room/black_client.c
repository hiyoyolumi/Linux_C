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
    int conn_fd;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZ];
    int len;

    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(conn_fd == -1)
    {
        my_err("socket error", __LINE__);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    if(connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        my_err("connect error", __LINE__);
    }

    while(1)
    {
        scanf("%s", buf);
        send(conn_fd, buf, strlen(buf), 0);

        len = recv(conn_fd, buf, sizeof(buf), 0);
        write(STDOUT_FILENO, buf, len);
        printf("\n");
    }

    close(conn_fd);

    return 0;
}