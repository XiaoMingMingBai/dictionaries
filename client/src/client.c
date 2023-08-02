#include "../include/client.h"

void do_register(int socketfd, MSG *msg)
{
    // 指定操作码
    msg->type = R;
    // 输入用户名
    printf("input your name:");
    scanf("%s", msg->name);
    // 输入密码
    printf("input your password:");
    scanf("%s", msg->data);
    // 发送数据
    send(socketfd, msg, sizeof(MSG), 0);
    // 接收数据并输出
    recv(socketfd, msg, sizeof(MSG), 0);
    printf("register : %s\n", msg->data);

    return;
}

int do_login(int socketfd, MSG *msg)
{
    // 设置操作码
    msg->type = L;
    // 输入用户名
    printf("input your name:");
    scanf("%s", msg->name);
    // 输入密码
    printf("input your password:");
    scanf("%s", msg->data);
    // 发送数据给服务器
    send(socketfd, msg, sizeof(MSG), 0);
    // 接收服务器发送的数据
    recv(socketfd, msg, sizeof(MSG), 0);

    // 判断是否登录成功
    if (strncmp(msg->data, "OK", 3) == 0)
    { //用3可以防止OK和OKkshdfkj
        // 登录成功返回1
        printf("login : OK\n");
        return 1;
    }

    // 登录失败返回0
    printf("login : %s\n", msg->data);
    return 0;
}

void do_query(int socketfd, MSG *msg)
{
    msg->type = Q;
    puts("-----------------------------");

    while (1)
    {
        printf("input word (if # is end): ");
        scanf("%s", msg->data);

        // 如果输入的是#，返回上一级
        if (strcmp(msg->data, "#") == 0)
        {
            break;
        }

        send(socketfd, msg, sizeof(MSG), 0);

        recv(socketfd, msg, sizeof(MSG), 0);
        printf(">>> %s\n", msg->data);
    }
    return;
}

void do_history(int socketfd, MSG *msg)
{
    msg->type = H;
    send(socketfd, msg, sizeof(MSG), 0);

    while (1)
    {
        recv(socketfd, msg, sizeof(MSG), 0);

        if (strcmp(msg->data, "**OVER**") == 0)
        {
            break;
        }

        printf("%s\n", msg->data);
    }

    return;
}
