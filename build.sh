#!/bin/bash
# 编译脚本 (Linux/macOS)

echo "正在编译图书管理系统..."

# 检查编译器
if ! command -v g++ &> /dev/null; then
    echo "错误: 未找到 g++ 编译器"
    echo "请安装 g++ 或使用 make 命令编译"
    exit 1
fi

# 编译
g++ -std=c++17 -Wall -Wextra -O2 -I library_core \
    Main.cpp \
    library_core/library.cpp \
    library_core/btree_book.cpp \
    library_core/btree_author.cpp \
    -o library_cli

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "运行程序: ./library_cli"
else
    echo "编译失败！"
    exit 1
fi

