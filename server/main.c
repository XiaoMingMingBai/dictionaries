#include "include/server.h"

int main(int argc, char *argv[])
{
    int listenfd, connectfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    pid_t pid;
    sqlite3 *db;

    struct sockaddr_in server_infor = {0};
    server_infor.sin_family = AF_INET;         // ipv4
    server_infor.sin_port = htons(8888);       // 端口号
    server_infor.sin_addr.s_addr = INADDR_ANY; // 任意ip地址
    // 绑定服务器套接字和网络信息结构体
    int ret = bind(listenfd, (struct sockaddr *)&server_infor, sizeof(server_infor));
    if (ret == -1)
    {
        perror("bind err\n");
        exit(1);
    }
    if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
    {
        printf("error : %s\n", sqlite3_errmsg(db));
        exit(-1);
    }
    if (listen(listenfd, 5) < 0)
    {
        perror("fail to listen");
        exit(-1);
    }

    signal(SIGCHLD, handler); // 处理僵尸进程

    while (1)
    {
        if ((connectfd = accept(listenfd, NULL, NULL)) < 0)
        {
            perror("fail to accept");
            exit(-1);
        }

        if ((pid = fork()) < 0)
        {
            perror("fail to fork");
            exit(-1);
        }
        else if (pid == 0) // 子进程执行处理代码
        {
            do_client(connectfd, db);
        }
        else // 父进程负责连接
        {
            close(connectfd);
        }
    }
    return 0;
}