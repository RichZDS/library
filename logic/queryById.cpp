#include "../controller/library.h"
#include "../datastruct/btree_book.h"


void queryById(const BTreeBook& books) {
    string id = inputLine("输入要查询的图书编号: ");
    Book* rec = const_cast<BTreeBook&>(books).search(id);
    if (!rec) {
        cout << "未找到该图书。\n";
        return;
    }
    printBook(*rec);
}
