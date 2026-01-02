@echo off
REM 编译脚本 (Windows)

echo 正在编译图书管理系统...

REM 检查编译器
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到 g++ 编译器
    echo 请安装 MinGW 或使用 make 命令编译
    pause
    exit /b 1
)

REM 编译
g++ -std=c++17 -Wall -Wextra -O2 -I library_core ^
    Main.cpp ^
    library_core/library.cpp ^
    library_core/btree_book.cpp ^
    library_core/btree_author.cpp ^
    -o library_cli.exe

if %errorlevel% equ 0 (
    echo 编译成功！
    echo 运行程序: library_cli.exe
) else (
    echo 编译失败！
    pause
    exit /b 1
)

pause

