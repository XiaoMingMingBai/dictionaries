# 基于TCP的在线词典

## 菜单
1. 注册
2. 登录
    1. 查询
    2. 历史
    3. 退出
3. 退出

## 使用说明
### 1. 创建数据库
```sql
.open file.db
create table usr (usr_name text primary key , usr_pwd text);
create table hisroty_word (usr_name text, time text, word text);
.quit
```
### 2. 创建单词本
```shell
touch dict.txt
# a                indef art one
# abacus           n.frame with beads that slide along parallel #rods, used for teaching numbers to children, and (in some countries) for counting
# abandon          v.  go away from (a person or thing or place) not intending to return; forsake; desert
# abandonment      n.  abandoning
# .....
```
### 3. 编译
```shell
make
```
### 4. 运行服务端
```shell
./ser IP PORT
```
### 5. 运行客户端
```shell
./cli IP PORT
```
