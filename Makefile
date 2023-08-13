# 编译客户端和服务器端
# 作者：xiaomingmingbai

# 变量定义
CC := gcc
CFLAGS := -lsqlite3
SER_OBJSS := $(wildcard *.c ./server/*.c)
CLI_OBJSS := $(wildcard *.c ./client/*.c)
SER_OBJS := $(patsubst %.c,%.o,$(SER_OBJSS))
CLI_OBJS := $(patsubst %.c,%.o,$(CLI_OBJSS))


all : cli ser

cli : $(CLI_OBJS)
	@echo "LD	$@"
	@$(CC) -o $@ $^

ser : $(SER_OBJS)
	@echo "LD	$@"
	@$(CC) -o $@ $^ $(CFLAGS)

%.o : %.c
	@echo "CC	$@"
	@$(CC) -c $< -o $@

clean :
	@echo "cleaning project"
	@rm -rf ./client/*.o ./server/*.o cli ser