#include <stdio.h>
#include <sys/socket.h>
#include <sqlite3.h>
#include <strings.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define DATABASE "./data/file.db"
#define DICT "./data/dict.txt"
#define N 20
#define SIZE 256
#define R 1
#define L 2
#define H 3
#define Q 4

typedef struct
{
	int type;
	char name[N];
	char data[SIZE];
} msg_t;

sqlite3 *db;

void do_register(int connfd, msg_t *pbuf) // 注册功能	R
{
	char *errmsg, **result;
	int nrow = 0, ncolumn;
	char sql[512] = {0};
	sprintf(sql, "select * from usr where usr_name = '%s';", pbuf->name);

	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n", errmsg);
		exit(-1);
	}
	if (nrow > 0) // 注册的用户名已经存在
	{
		sprintf(pbuf->data, "\n      用户名已经存在，请重新注册(̿▀̿ ̿Ĺ̯̿̿▀̿ ̿)\n");
		send(connfd, pbuf, sizeof(msg_t), 0);
	}
	else // 用户名不存在，可以注册
	{
		sprintf(sql, "insert into usr values ('%s','%s');", pbuf->name, pbuf->data);
		if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			perror("sqlite3_exec");
			printf("error:%s\n", errmsg);
			exit(-1);
		}
		sprintf(pbuf->data, "\n     %s,恭喜您,注册成功(｡･ω･｡)ﾉ♡", pbuf->name); // 返回注册成功信息给用户
		send(connfd, pbuf, sizeof(msg_t), 0);
	}
}

void do_login(int connfd, msg_t *pbuf) // 登录	L
{
	char *errmsg;
	char sql[512] = {0};
	int nrow = 0, ncolumn = 0;
	char **result;

	sprintf(sql, "select * from usr where usr_name = '%s' and usr_pwd = '%s';", pbuf->name, pbuf->data);
	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		perror("sqlite3_get_table");
		printf("error:%s\n", sqlite3_errmsg(db));
		exit(-1);
	}
	if (nrow > 0)
	{
		sprintf(pbuf->data, "\n      %s,恭喜您,登录成功(｡･ω･｡)ﾉ♡\n", pbuf->name);
		pbuf->type = 8;
	}
	else
	{
		sprintf(pbuf->data, "\n  \033[1;31m用户名或密码错误,请重新输入o(#￣▽￣)==O))￣0￣\")o金钢飞拳~!!\033[0m\n");
	}
	send(connfd, pbuf, sizeof(msg_t), 0);
	// sqlite3_free_table(result);
}

// 获取系统时间
char get_date(char *date)
{
	struct tm *tp = NULL;
	time_t rawtime;
	time(&rawtime);
	tp = localtime(&rawtime);
	sprintf(date, "%02d-%02d-%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1,
			tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
}

void do_query(int connfd, msg_t *pbuf) // 请求	Q
{
	int ret;
	char line[200]; // 用来保存读取的单词及解释
	char time[100]; // 用来保存系统时间
	char *errmsg;
	char sql[512] = {0};
	char word[200];
	strcpy(word, pbuf->data);
	int len = strlen(word);
	char *p = line + len;
	FILE *fp = fopen(DICT, "r");
	if (fp == NULL)
	{
		perror("fopen failed");
		sprintf(pbuf->data, "抱歉,服务器打开词典文本失败 :(\n");
		send(connfd, pbuf, sizeof(msg_t), 0);
		exit(-1);
	}

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		ret = strncmp(line, word, len);
		if (ret == 0 && line[len] == ' ') // 跳过单词及空格，直接定位到单词解释
		{
			while (*p == ' ')
				p++;
			strcpy(pbuf->data, p);
			send(connfd, pbuf, sizeof(msg_t), 0);
			get_date(time);

			// 将查找过的单词信息写进历史记录表中以备客户历史查找请求
			sprintf(sql, "insert into history_word values('%s','%s','%s');", pbuf->name, time, word);
			if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
			{
				perror("sqlite3_exec");
				printf("error:%s\n", errmsg);
				exit(-1);
			}
			fclose(fp);
			return (void)0;
		}
		else if (ret < 0)
			continue;
		else // 遍历到对应单词位置仍未查到，不用再继续查找，跳出循环
			break;
	}
	sprintf(pbuf->data, "抱歉，没有找到该单词的意思(⍥(⍥(⍥(⍥(⍥;;) 惊呆惹\n");
	send(connfd, pbuf, sizeof(msg_t), 0);
	fclose(fp);
}
// 回调函数，每找到一条记录就执行一次该函数
int history_callback(void *arg, int f_column, char **f_value, char **f_name)
{
	int i, connfd = *(int *)arg;
	msg_t pbuf;
	sprintf(pbuf.data, "%s , %s", f_value[1], f_value[2]);
	send(connfd, &pbuf, sizeof(pbuf), 0);
	return 0;
}

int do_history(int connfd, msg_t *pbuf) // 历史查找	H
{
	char sqlstr[128] = {0};
	char *errmsg;

	sprintf(sqlstr, "select * from history_word where usr_name = '%s';", pbuf->name);
	if (sqlite3_exec(db, sqlstr, history_callback, (void *)&connfd, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n", errmsg);
		return -1;
	}
	else
		printf("请求成功.\n");
	pbuf->data[0] = '0';
	send(connfd, pbuf, sizeof(msg_t), 0);
	return 1;
}

void handler(int signo)
{
	wait(NULL);
}

// TCP
int main(int argc, char *argv[])
{
	// 参数1：./a.out  2：ip地址 3：端口
	if (argc < 3)
	{
		printf("%s <ip> <port>\n", argv[0]);
		return -1;
	}
	// 打开.db文件
	if ((sqlite3_open(DATABASE, &db)) < 0)
	{
		perror("sqlite3_open");
		return -1;
	}
	// 创建usr表保存用户名和密码
	char *errmsg;

	// 创建套接字描述符
	int lisenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lisenfd < 0)
	{
		perror("socket");
		return -1;
	}
	// 绑定
	struct sockaddr_in serveraddr, clientaddr;
	int peerlen = sizeof(struct sockaddr);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

	if (bind(lisenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("bind");
		return -1;
	}

	// 监听
	if (listen(lisenfd, 10) < 0)
	{
		perror("listen");
		return -1;
	}

	// 处理僵尸进程
	signal(SIGCHLD, handler);

	msg_t buf;
	while (1)
	{
		// 创建新的套接字描述符
		int connfd = accept(lisenfd, (struct sockaddr *)&serveraddr, &peerlen);
		if (connfd < 0)
		{
			perror("accept");
			return -1;
		}
		// 创建子进程
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("fork");
			return -1;
		}
		else if (pid == 0) // 子进程
		{
			while (1)
			{
				if (recv(connfd, &buf, sizeof(buf), 0) > 0)
				{
					printf("recv:%d:%s %s\n", buf.type, buf.name, buf.data);
					switch (buf.type)
					{
					case R:
						do_register(connfd, &buf);
						break;
					case L:
						do_login(connfd, &buf);
						break;
					case Q:
						do_query(connfd, &buf);
						break;
					case H:
						do_history(connfd, &buf);
						break;
					default:
						break;
					}
				}
				else
				{
					perror("recv failed");
					exit(-1);
				}
			}
		}
		else // 父进程退出
			close(connfd);
	}
	return 0;
}
