# 编译客户端和服务器端
# 作者：xiaomingmingbai

# 变量定义
CROSS_COMPILE = arm-linux-gnueabihf-
NAME = client server
CC = $(CROSS_COMPILE)gcc
ser := ser
cli := cli

# 定义编译程序变量
#     -g : 添加gdb的调试信息    -marm : 编译生成arm指令    -Wall : 显示所有警告    -O0 : 代码优化等级，不优化
#     -mabi=apcs-gnu : 架构    -mfpu=neon -mfloat-abi=softfp : 浮点数    -fno-builtin : 不使用内建函数
# 	  -nostdinc : 不适用标准头文件    -I./include : 指定头文件路径	-lsqlite3 : 链接sqlite3库
SER_CFLAGS += -I./server/include -I./common/include -lsqlite3
CLI_CFLAGS += -I./client/include -I./common/include

# 展开目录下所有符合要求的文件名并赋值给变量
SER_OBJS := $(wildcard server/src/*.c) $(wildcard server/*.c)
CLI_OBJS := $(wildcard client/src/*.c) $(wildcard client/*.c)
# 将变量中所有符合要求的字符串进行替换
SER_OBJ := $(patsubst %.c, %.o, $(SER_OBJS))
CLI_OBJ := $(patsubst %.c, %.o, $(CLI_OBJS))

# 构建目标
all:clscli server client
	@echo "  BUILD   all"

server:clsser $(ser)
$(ser):$(SER_OBJ)
	@echo "  LD      $^"
	@gcc $^ -o $@ -lsqlite3
%.o:%.c
	@echo "  CC      $@"
	@gcc $(SER_CFLAGS) -c $< -o $@

client:clscli $(cli)
$(cli):$(CLI_OBJ)
	@echo "  LD      $^"
	@gcc $(SER_CFLAGS) $^ -o $@
%.o:%.c
	@echo "  CC      $@"
	@gcc $(CLI_CFLAGS) -c $< -o $@

# 清除所有生成的文件
clsall:clsser clscli
	@echo "  CLEAN   all"

clsser:
	@echo "  CLEAN   server"
	@rm -rf $(ser) $(SER_OBJ)

clscli:
	@echo "  CLEAN   client"
	@rm -rf $(cli) $(CLI_OBJ)
