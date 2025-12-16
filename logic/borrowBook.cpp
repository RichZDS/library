#include "../controller/library.h"
#include "../datastruct/btree_book.h"


void borrowBook(BTreeBook& books) {
    string id = inputLine("输入借阅的图书编号: ");
    Book* rec = books.search(id);
    if (!rec) {
        cout << "未找到该图书。\n";
        return;
    }
    Book& book = *rec;
    if (book.availableCopies <= 0) {
        cout << "库存不足，无法借阅。\n";
        return;
    }
    book.availableCopies -= 1;
    cout << "借阅成功。\n";
    printBook(book);
}
