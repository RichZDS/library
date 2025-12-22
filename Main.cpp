#include "library_core/library.h"
#include "library_core/btree_book.h"
#include "library_core/btree_author.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

int main() {
    SetConsoleOutputCP(65001);  // UTF-8
    SetConsoleCP(65001);        // UTF-8
    system("chcp 65001 > nul");
    
    BTreeBook books;
    BTreeAuthor authorIndex;
    // 预置图书（不超过5本）
    {
        Book b1; b1.id="B001"; b1.title="数据结构"; b1.author="王小明"; b1.publisher="清华大学出版社"; b1.price=59.8; b1.publishDate="2020-09"; b1.totalCopies=5; b1.availableCopies=b1.totalCopies; addOrAcquireBook(books, authorIndex, b1);
        Book b2; b2.id="B002"; b2.title="算法导论"; b2.author="Cormen"; b2.publisher="MIT Press"; b2.price=98.0; b2.publishDate="2009-07"; b2.totalCopies=3; b2.availableCopies=b2.totalCopies; addOrAcquireBook(books, authorIndex, b2);
        Book b3; b3.id="B003"; b3.title="操作系统"; b3.author="汤子瀛"; b3.publisher="高等教育出版社"; b3.price=66.0; b3.publishDate="2018-03"; b3.totalCopies=4; b3.availableCopies=b3.totalCopies; addOrAcquireBook(books, authorIndex, b3);
    }
    
    auto showRoleMenu = [](){
        cout << "\n========================================" << endl;
        cout << "        图书管理系统" << endl;
        cout << "========================================" << endl;
        cout << "1. 管理员" << endl;
        cout << "2. 用户" << endl;
        cout << "0. 退出" << endl;
        cout << "========================================" << endl;
        cout << "请选择模式: ";
    };

    auto userMenu = [](){
        cout << "\n========================================" << endl;
        cout << "              用户模式" << endl;
        cout << "========================================" << endl;
        cout << "1. 借书" << endl;
        cout << "2. 还书" << endl;
        cout << "3. 预约书籍" << endl;
        cout << "4. 查看所有图书信息" << endl;
        cout << "5. 作者索引查询" << endl;
        cout << "6. 书号查询" << endl;
        cout << "7. 书名查询" << endl;
        cout << "0. 返回" << endl;
        cout << "========================================" << endl;
        cout << "请选择操作: ";
    };

    auto adminMenu = [](){
        cout << "\n========================================" << endl;
        cout << "             管理员模式" << endl;
        cout << "========================================" << endl;
        cout << "1. 图书录入" << endl;
        cout << "2. 图书清除" << endl;
        cout << "3. 查询所有借阅者信息" << endl;
        cout << "4. 查询所有预约者信息" << endl;
        cout << "5. 查看某书借阅与预约情况" << endl;
        cout << "6. 作者索引查询" << endl;
        cout << "7. 书号查询" << endl;
        cout << "8. 书名查询" << endl;
        cout << "0. 返回" << endl;
        cout << "========================================" << endl;
        cout << "请选择操作: ";
    };

    while (true) {
        clearScreen();
        showRoleMenu();
        int role;
        if (!(cin >> role)) { clearInput(); continue; }
        clearInput();
        if (role == 0) { cout << "感谢使用，再见！" << endl; return 0; }
        if (role == 1) {
            while (true) {
                clearScreen();
                adminMenu();
                int c; if (!(cin >> c)) { clearInput(); continue; }
                clearInput();
                if (c == 0) break;
                clearScreen();
                switch (c) {
                    case 1: addOrAcquireBook(books, authorIndex); break;
                    case 2: removeBook(books, authorIndex); break;
                    case 3: listAllBorrowers(books); break;
                    case 4: listAllReservers(books); break;
                    case 5: viewBorrowReserveById(books); break;
                    case 6: queryByAuthor(books, authorIndex); break;
                    case 7: queryById(books); break;
                    case 8: queryByTitle(books); break;
                    default: cout << "无效的选择！" << endl; break;
                }
                cout << "\n按回车键继续...";
                cin.get();
            }
        } else if (role == 2) {
            // 用户登录：一次输入姓名与借阅证号
            clearScreen();
            cout << R"(,----,   ,--,                       
                                             ,/   .`|,---.'|                       
   .--.--.       ,---,                     ,`   .'  :|   | :      ,---,    ,---,.  
  /  /    '.   .'  .' `\           ,--,  ;    ;     /:   : |   ,`--.' |  ,'  .'  \ 
 |  :  /`. / ,---.'     \        ,'_ /|.'___,/    ,' |   ' :   |   :  :,---.' .' | 
 ;  |  |--`  |   |  .`\  |  .--. |  | :|    :     |  ;   ; '   :   |  '|   |  |: | 
 |  :  ;_    :   : |  '  |,'_ /| :  . |;    |.';  ;  '   | |__ |   :  |:   :  :  / 
  \  \    `. |   ' '  ;  :|  ' | |  . .`----'  |  |  |   | :.'|'   '  ;:   |    ;  
   `----.   \'   | ;  .  ||  | ' |  | |    '   :  ;  '   :    ;|   |  ||   :     \ 
   __ \  \  ||   | :  |  ':  | | :  ' ;    |   |  '  |   |  ./ '   :  ;|   |   . | 
  /  /`--'  /'   : | /  ; |  ; ' |  | '    '   :  |  ;   : ;   |   |  ''   :  '; | 
 '--'.     / |   | '` ,/  :  | : ;  ; |    ;   |.'   |   ,/    '   :  ||   |  | ;  
   `--'---'  ;   :  .'    '  :  `--'   \   '---'     '---'     ;   |.' |   :   /   
             |   ,.'      :  ,      .-./                       '---'   |   | ,'    
             '---'         `--`----'                                   `----'      
                                                                                   )" << endl;
            string userName = inputLine("请输入姓名: ");
            string userCard = inputLine("请输入借阅证号: ");
            if (userName.empty() || userCard.empty()) {
                cout << "信息不完整，返回主菜单。" << endl;
                cout << "\n按回车键继续..."; cin.get();
                continue;
            }
            while (true) {
                clearScreen();
                userMenu();
                int c; if (!(cin >> c)) { clearInput(); continue; }
                clearInput();
                if (c == 0) break;
                clearScreen();
                switch (c) {
                    case 1: {
                        string id = inputLine("请输入书号: ");
                        if (borrowBookById(books, id, userCard, userName)) {
                            cout << "借阅成功！" << endl;
                        } else {
                            cout << "借阅失败（书号不存在或库存为零）！" << endl;
                        }
                    } break;
                    case 2: {
                        string id = inputLine("请输入书号: ");
                        if (returnBookById(books, id, userCard)) {
                            cout << "归还成功！" << endl;
                        } else {
                            cout << "归还失败（书号不存在或无未归还记录）！" << endl;
                        }
                    } break;
                    case 3: {
                        string id = inputLine("请输入书号: ");
                        if (reserveBookById(books, id, userCard, userName)) {
                            cout << "预约成功！" << endl;
                        } else {
                            cout << "预约失败（书号不存在或信息不完整）！" << endl;
                        }
                    } break;
                    case 4: listAll(books); break;
                    case 5: queryByAuthor(books, authorIndex); break;
                    case 6: queryById(books); break;
                    case 7: queryByTitle(books); break;
                    default: cout << "无效的选择！" << endl; break;
                }
                cout << "\n按回车键继续...";
                cin.get();
            }
        }
    }
    return 0;
}
