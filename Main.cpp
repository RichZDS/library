#define NOMINMAX
#include <windows.h>
#include "./controller/library.h"
#include "./datastruct/btree_book.h"
#include "./datastruct/btree_author.h"


int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    BTreeBook books;
    BTreeAuthor authorIndex;

    while (true) {
        showMenu();
        int choice = inputInt("请选择操作: ");
        switch (choice) {
            case 1:
                addOrAcquireBook(books, authorIndex);
                break;
            case 2:
                removeBook(books, authorIndex);
                break;
            case 3:
                borrowBook(books);
                break;
            case 4:
                returnBook(books);
                break;
            case 5:
                queryById(books);
                break;
            case 6:
                queryByAuthor(books, authorIndex);
                break;
            case 7:
                listAll(books);
                break;
            case 0:
                cout << "退出系统。\n";
                return 0;
            default:
                cout << "无效选择，请重新输入。\n";
                break;
        }
    }
}
