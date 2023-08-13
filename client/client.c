#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<strings.h>
#include<stdlib.h>

#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>

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
}MSG;
//用户注册
void do_register(int sockfd,MSG *pbuf)
{
	pbuf->type = R;
	printf("用户名:");
	scanf("%s",pbuf->name);
	printf("密  码:\033[8;33m");
	scanf("%s",pbuf->data);

	send(sockfd,pbuf,sizeof(MSG),0);
	recv(sockfd,pbuf,sizeof(MSG),0);
	printf("\033[0m %s\n",pbuf->data);
	sleep(1);
}
//用户登录
int do_login(int sockfd,MSG *pbuf)
{
	pbuf->type = L;
	printf("用户名:");
	scanf("%s",pbuf->name);
	printf("密  码:\033[8;33m");
	scanf("%s",pbuf->data);
	send(sockfd,pbuf,sizeof(MSG),0);

	recv(sockfd,pbuf,sizeof(MSG),0);
	if(pbuf->type == 8)
	{	
		printf("\033[0m %s\n",pbuf->data); 
		sleep(1);
		return 1;
	}
	else
	{
		printf("\033[0m %s\n",pbuf->data);
		return 0;
	}
}

//查找单词
void do_query(int sockfd,MSG *pbuf)
{
	pbuf->type = Q;
	printf("请输入你要查询的单词(#结束):");
	while(1)
	{
		scanf("%s",pbuf->data);
		getchar();
		if(strcmp(pbuf->data,"#")==0)
			break;
		send(sockfd,pbuf,sizeof(MSG),0);

		recv(sockfd,pbuf,sizeof(MSG),0);

		printf("╭( ′• o •′ )╭☞就是这个意思：%s\n",pbuf->data);
		sleep(1);
		printf("请输入你要查询的单词:");
	}
}

//查询历史单词
void do_history(int sockfd,MSG *pbuf)
{
	pbuf->type = H;
	send(sockfd,pbuf,sizeof(MSG),0);
	while(1)
	{
		recv(sockfd,pbuf,sizeof(MSG),0);
		if(pbuf->data[0] == '0')		//历史单词全部接受完成
			break;
		printf("%s\n",pbuf->data);
	}
	sleep(0.2);
}

//二级菜单
void menu_2(int sockfd,MSG *pbuf)
{
	while(1)
	{
		printf("\033[1;33m****************************************\n");//字体格式设置
		printf("*            欢迎使用电子词典          *\n");
		printf("****************************************\n");
		printf("*  1:查询单词  2:查询历史单词  3:退出  *\n");
		printf("****************************************\033[0m\n");//字体格式设置关闭
		printf("请选择:");
		scanf("%d",&pbuf->type);
		switch(pbuf->type)
		{
			case 1: 
				do_query(sockfd,pbuf);
				break;
			case 2:
				do_history(sockfd,pbuf);
				break;
			case 3:
				printf("      即将退出，欢迎您再次使用(￣o￣) . z Z\n");
				send(sockfd,pbuf,sizeof(MSG),0);
				sleep(1);
				close(sockfd);
				exit(-1);
			default:
				printf("\033[1;31m错误选项!\033[0m");
				break;
		}
	}
}

//一级菜单
void menu_1(int sockfd,MSG *pbuf)
{
	int mode;
	while(1)
	{
		printf("\033[1;36m****************************************\n");
		printf("*              电子词典项目            *\n");
		printf("****************************************\n");
		printf("*     1: 注册   2: 登录   3: 退出      *\n");
		printf("****************************************\033[0m\n");
		printf("请选择:");
		scanf("%d",&mode);

		switch(mode)
		{
		case 1: 
			do_register(sockfd,pbuf);
			break;
		case 2:
			while(do_login(sockfd,pbuf) != 1)
				continue;
			menu_2(sockfd,pbuf);
			break;
		case 3:
			printf("      即将退出，欢迎您再次使用(￣o￣) . z Z\n");
			sleep(1);
			close(sockfd);
			exit(-1);
		default:
				printf("\033[1;31m错误选项!\033[0m");
				break;
		}
	}
}

int main(int argc, const char *argv[])
{
	if ( argc != 3 )
	{
		printf("%s <ip> <port>\n",argv[0]);
		return -1;
	}
	
	int sockfd;
	if(( sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	
	if( connect(sockfd, (struct sockaddr *)&serveraddr,sizeof(struct sockaddr)) < 0 )
	{
		perror("connect");
		return -1;
	}
	//与服务器连接完成后，进入功能模式选择菜单界面
	MSG buf;
	menu_1(sockfd,&buf);
	return 0;
}
