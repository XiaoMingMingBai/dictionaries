#include "include/client.h"

int main(int argc, char *argv[])
{
    int socketfd;
    struct sockaddr_in server_addr;
    MSG msg;

    // 创建流式套接字
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        perror("socket err\n");
        exit(-1);
    }
    // 创建网络信息结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("192.168.250.100");
    // 连接服务器
    int ret = connect(socketfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect err\n");
        exit(-1);
    }

    int choose = 0;
    while (1)
    {
        printf("************************************\n");
        printf("* 1: register 2: login 3: quit *\n");
        printf("************************************\n");
        printf("please choose : ");

        if (scanf("%d", &choose) <= 0)
        {
            perror("scanf");
            exit(-1);
        }

        switch (choose)
        {
        case 1:
            do_register(socketfd, &msg);
            break;
        case 2:
            // 执行登录函数，执行完毕后通过返回值决定是否要跳转到下一个菜单
            if (do_login(socketfd, &msg) == 1)
            {
                goto next;
            }
            break;
        case 3:
            close(socketfd);
            exit(0);
        }
    }
next:
    while (1)
    {
        printf("************************************\n");
        printf("* 1: query 2: history 3: quit *\n");
        printf("************************************\n");
        printf("please choose : ");

        if (scanf("%d", &choose) <= 0)
        {
            perror("scanf");
            exit(-1);
        }
        switch (choose)
        {
        case 1:
            do_query(socketfd, &msg);
            break;
        case 2:
            do_history(socketfd, &msg);
            break;
        case 3:
            close(socketfd);
            exit(0);
        }
    }
    return 0;
}
