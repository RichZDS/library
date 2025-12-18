@echo off
echo 正在编译 Main_API.cpp...

REM 使用g++编译（如果已安装）
where g++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo 使用 g++ 编译器...
    g++ -std=c++11 -I. Main_API.cpp datastruct/btree_book.cpp datastruct/btree_author.cpp -o Library_API.exe
    if %ERRORLEVEL% == 0 (
        echo 编译成功！生成 Library_API.exe
    ) else (
        echo 编译失败！请检查错误信息
        exit /b 1
    )
) else (
    echo 未找到 g++ 编译器
    echo.
    echo 请使用以下方法之一：
    echo 1. 安装 MinGW 或 MSYS2 以获取 g++
    echo 2. 使用 Visual Studio 编译器 (cl.exe)
    echo 3. 使用其他C++编译器
    echo.
    echo Visual Studio 编译命令示例：
    echo cl /EHsc /I. Main_API.cpp datastruct/btree_book.cpp datastruct/btree_author.cpp /Fe:Library_API.exe
    exit /b 1
)

pause

