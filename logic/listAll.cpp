#include "../controller/library.h"
#include "../datastruct/btree_book.h"


void listAll(const BTreeBook& books) {
    std::vector<Book> items;
    const_cast<BTreeBook&>(books).inorder(items);
    if (items.empty()) {
        cout << "当前没有图书记录。\n";
        return;
    }
    for (const auto& b : items) {
        printBook(b);
        cout << "--------------------\n";
    }
}
