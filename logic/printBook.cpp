#include "../controller/library.h"


void printBook(const Book& book) {
    cout << "编号: " << book.id << "\n";
    cout << "书名: " << book.title << "\n";
    cout << "作者: " << book.author << "\n";
    cout << "出版社: " << book.publisher << "\n";
    cout << "单价: " << book.price << "\n";
    cout << "总册数: " << book.totalCopies << "\n";
    cout << "可借册数: " << book.availableCopies << "\n";
}

