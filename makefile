TAR = bin/test  # 目标文件
INC = -I ./include 
LIB = -L ./ -lfont

SRC = $(wildcard src/*.cpp)  # 获取当前目录下的所有 .cpp 和 .c 文件
MAIN = main/main.cpp  # 主文件（包含 main 函数）
VERSION = -std=c++11  # 编译版本指令
# CC = gcc
CC = arm-linux-g++  # 交叉编译工具（使用 arm-linux-g++ 编译器）

$(TAR): $(MAIN) $(SRC)
	$(CC) $(VERSION) $^  -o $@ $(INC) $(LIB)  
# 使用编译器将所有文件编译为目标文件$(TAR)

run:
	./$(TAR)  
# 运行目标文件

.PHONY: clean

clean:
	$(RM) $(TAR)  
# 删除目标文件
