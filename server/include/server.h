#if !defined(__SERVER_H__)
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define N 16
#define R 1 // user register
#define L 2 // user login
#define Q 3 // query word
#define H 4 // history record

#define DATABASE "./db/my.db"
typedef struct
{
int type;
char name[N];
char data[256]; // password or word
} MSG;
void handler(int sig);
void do_register(int connectfd, MSG *msg, sqlite3 *db);
void do_login(int connectfd, MSG *msg, sqlite3 *db);
void do_query(int connectfd, MSG *msg, sqlite3 *db);
void do_history(int connectfd, MSG *msg, sqlite3 *db);
void do_client(int connectfd, sqlite3 *db);
int do_searchword(int connectfd, MSG *msg);
void getdata(char data[]);
int history_callback(void *arg, int f_num, char **f_value, char **f_name);


#endif // __SERVER_H__
