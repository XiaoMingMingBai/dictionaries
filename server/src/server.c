#include "../include/server.h"

void handler(int sig)
{
    wait(NULL);
}

void do_client(int connectfd, sqlite3 *db)
{
    MSG msg;
    while (recv(connectfd, &msg, sizeof(MSG), 0) > 0) // receive request
    {
        printf("type = %d\n", msg.type);
        printf("type = %s\n", msg.data);
        switch (msg.type)
        {
        case R:
            do_register(connectfd, &msg, db);
            break;
        case L:
            do_login(connectfd, &msg, db);
            break;
        case Q:
            do_query(connectfd, &msg, db);
            break;
        case H:
            do_history(connectfd, &msg, db);
            break;
        }
    }
    printf("client quit\n");
    exit(0);
    return;
}

void do_register(int connectfd, MSG *msg, sqlite3 *db)
{
    char sqlstr[512] = {0};
    char *errmsg;

    // 使用sqlite3_exec函数调用插入函数判断是否能够插入成功
    // 由于用户名设置为主键，所以如果用户名已经存在就会报错
    sprintf(sqlstr, "insert into usr values('%s', '%s')", msg->name, msg->data);
    if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        sprintf(msg->data, "user %s already exist!!!", msg->name);
    }
    else
    {
        strcpy(msg->data, "OK");
    }

    send(connectfd, msg, sizeof(MSG), 0);

    return;
}

void do_login(int connectfd, MSG *msg, sqlite3 *db)
{
    char sqlstr[512] = {0};
    char *errmsg, **result;
    int nrow, ncolumn;

    // 通过sqlite3_get_table函数查询记录是否存在
    sprintf(sqlstr, "select * from usr where name = '%s' and pass = '%s'", msg->name, msg->data);
    if (sqlite3_get_table(db, sqlstr, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
    {
        printf("error : %s\n", errmsg);
    }
    ////通过通过nrow参数判断是否能够查询到疾记录，如果值为0，则查询不到，如果值为非0，则查询到
    if (nrow == 0)
    {
        strcpy(msg->data, "name or password is wrony!!!");
    }
    else
    {
        strncpy(msg->data, "OK", 256);
    }

    send(connectfd, msg, sizeof(MSG), 0);

    return;
}

void do_query(int connectfd, MSG *msg, sqlite3 *db)
{

    char sqlstr[512], *errmsg;
    int found = 0;
    char date[128], word[128];

    strcpy(word, msg->data);

    // 通过found保存查询结果
    found = do_searchword(connectfd, msg);

    // 如果执行成功，还需要保存历史记录
    if (found == 1)
    {
        // 获取时间
        getdata(date);
        // 通过sqlite3_exec函数插入数据
        sprintf(sqlstr, "insert into record values('%s', '%s', '%s')", msg->name, date, word);
        if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK)
        {
            printf("error : %s\n", errmsg);
        }
    }

    send(connectfd, msg, sizeof(MSG), 0);

    return;
}

int do_searchword(int connectfd, MSG *msg)
{
    FILE *fp;
    char temp[300];
    char *p;
    int len, result;
    // 保存单词的长度
    len = strlen(msg->data);
    // 打开保存单词的文件
    if ((fp = fopen("dict.txt", "r")) == NULL)
    {
        strcpy(msg->data, "dict can not open");
        send(connectfd, msg, sizeof(MSG), 0);
    }
    // printf("query word is %s len = %d\n", msg->data, len);

    // 每次读取一行内容
    int flags = 0;
    while (fgets(temp, 300, fp) != NULL)
    {
        // 比较单词
        result = strncmp(msg->data, temp, len);

        if (result == 0 && temp[len] == ' ')
        {
            // p保存单词后面第一个空格的首地址
            p = temp + len;

            ////移动移动pp，让，让pp保存解释的第一个字符的首地址 保存解释的第一个字符的首地址
            while (*p == ' ')
            {
                p++;
            }

            // 将解释保存在data里面
            memcpy(msg->data, p, strlen(p));

            fclose(fp);
            return 1;
        }
    }

    strcpy(msg->data, "not found");
    fclose(fp);
    return 0;
}

void getdata(char *data)
{
    time_t t;
    struct tm *tp;

    time(&t);
    tp = localtime(&t);
    sprintf(data, "%d-%d-%d %d:%d:%d", 1900 + tp->tm_year, 1 + tp->tm_mon, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec);
}

void do_history(int connectfd, MSG *msg, sqlite3 *db)
{
    char sqlstr[128], *errmsg;

    // 查询历史表
    sprintf(sqlstr, "select * from record where name = '%s'", msg->name);
    if (sqlite3_exec(db, sqlstr, history_callback, (void *)&connectfd, &errmsg) != SQLITE_OK)
    {
        printf("error : %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    // 发送结束标志
    strcpy(msg->data, "**OVER**");
    send(connectfd, msg, sizeof(MSG), 0);

    return;
}

// 通过回调函数发送时间和单词
int history_callback(void *arg, int f_num, char **f_value, char **f_name)
{
    int connectfd;
    MSG msg;
    connectfd = *(int *)arg;
    sprintf(msg.data, "%s : %s", f_value[1], f_value[2]);
    send(connectfd, &msg, sizeof(msg), 0);
    return 0;
}
