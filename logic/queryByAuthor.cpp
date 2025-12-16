#include "../controller/library.h"
#include "../datastruct/btree_book.h"
#include "../datastruct/btree_author.h"


void queryByAuthor(const BTreeBook& books, const BTreeAuthor& authorIndex) {
    string author = inputLine("输入作者姓名: ");
    auto ids = authorIndex.getIds(author);
    bool found = false;
    for (const auto& bid : ids) {
        Book* rec = const_cast<BTreeBook&>(books).search(bid);
        if (rec) {
            printBook(*rec);
            cout << "--------------------\n";
            found = true;
        }
    }
    if (!found) {
        cout << "未找到该作者的图书。\n";
    }
}
