#include "../controller/library.h"
#include "../datastruct/btree_book.h"


void returnBook(BTreeBook& books) {
    string id = inputLine("输入归还的图书编号: ");
    Book* rec = books.search(id);
    if (!rec) {
        cout << "未找到该图书。\n";
        return;
    }
    Book& book = *rec;
    if (book.availableCopies >= book.totalCopies) {
        cout << "该图书的可借册数已达到总册数。\n";
        return;
    }
    book.availableCopies += 1;
    cout << "归还成功。\n";
    printBook(book);
}
