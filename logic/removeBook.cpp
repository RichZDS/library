#include "../controller/library.h"
#include "../datastruct/btree_book.h"
#include "../datastruct/btree_author.h"


void removeBook(BTreeBook& books, BTreeAuthor& authorIndex) {
    string id = inputLine("输入要删除的图书编号: ");
    Book* rec = books.search(id);
    if (!rec) {
        cout << "未找到该图书。\n";
        return;
    }
    string author = rec->author;
    books.remove(id);

    authorIndex.removePair(author, id);

    cout << "图书记录已删除。\n";
}
