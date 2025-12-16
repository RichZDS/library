#include "../controller/library.h"
#include "../datastruct/btree_book.h"
#include "../datastruct/btree_author.h"

void addOrAcquireBook(BTreeBook& books, BTreeAuthor& authorIndex) {
    string id = inputLine("输入图书编号: ");
    if (id.empty()) {
        cout << "编号不能为空。\n";
        return;
    }
    Book* exist = books.search(id);
    if (exist) {
        int addCopies = inputInt("该图书已存在，输入新增册数: ");
        if (addCopies <= 0) {
            cout << "新增册数必须大于 0。\n";
            return;
        }
        exist->totalCopies += addCopies;
        exist->availableCopies += addCopies;
        cout << "已更新馆藏数量。\n";
        return;
    }

    Book book;
    book.id = id;
    book.title = inputLine("输入书名: ");
    book.author = inputLine("输入作者: ");
    book.publisher = inputLine("输入出版社: ");
    book.price = inputDouble("输入单价: ");
    book.totalCopies = inputInt("输入馆藏总册数: ");
    if (book.totalCopies < 0) {
        book.totalCopies = 0;
    }
    book.availableCopies = book.totalCopies;

    books.insert(book);
    authorIndex.add(book.author, id);
    cout << "新书已入库。\n";
}
