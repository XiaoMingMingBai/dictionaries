#if !defined(__CLIENT_H__)
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define N 16
#define R 1 // user register
#define L 2 // user login
#define Q 3 // query word
#define H 4 // history record

#define DATABASE "my.db"

typedef struct
{
int type;
char name[N];
char data[256]; // password or word or remark
} MSG;

void do_register(int socketfd, MSG *msg);
int do_login(int socketfd, MSG *msg);
void do_query(int socketfd, MSG *msg);
void do_history(int socketfd, MSG *msg);

#endif // __CLIENT_H__
