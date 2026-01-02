# 图书管理系统 Makefile
# 使用 C++17 标准编译

# 编译器
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# 目录
SRC_DIR = .
LIB_DIR = library_core

# 源文件
MAIN_SRC = Main.cpp
LIB_SRCS = $(LIB_DIR)/library.cpp $(LIB_DIR)/btree_book.cpp $(LIB_DIR)/btree_author.cpp

# 目标文件
OBJS = $(MAIN_SRC:.cpp=.o) $(LIB_SRCS:.cpp=.o)

# 可执行文件
TARGET = library_cli

# 默认目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# 编译主程序
Main.o: Main.cpp
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c Main.cpp -o Main.o

# 编译库文件
$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)

# 运行
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

