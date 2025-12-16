#include "btree_book.h"
#include <algorithm>

BTreeBook::BTreeBook(int t): t(t), root(nullptr) {}
BTreeBook::~BTreeBook() { destroy(root); }

void BTreeBook::destroy(BTreeBookNode* x) {
    if (!x) return;
    for (auto c : x->children) destroy(c);
    delete x;
}

Book* BTreeBook::search(const std::string& id) const {
    return searchNode(root, id);
}

Book* BTreeBook::searchNode(BTreeBookNode* x, const std::string& id) const {
    if (!x) return nullptr;
    int i = 0;
    while (i < (int)x->keys.size() && id > x->keys[i]) ++i;
    if (i < (int)x->keys.size() && id == x->keys[i]) {
        return const_cast<Book*>(&x->records[i]);
    }
    if (x->leaf) return nullptr;
    return searchNode(x->children[i], id);
}

void BTreeBook::splitChild(BTreeBookNode* x, int i) {
    BTreeBookNode* y = x->children[i];
    BTreeBookNode* z = new BTreeBookNode(y->leaf);
    // move t-1 keys from y to z
    for (int j = 0; j < t - 1; ++j) {
        z->keys.push_back(y->keys[j + t]);
        z->records.push_back(y->records[j + t]);
    }
    if (!y->leaf) {
        for (int j = 0; j < t; ++j) {
            z->children.push_back(y->children[j + t]);
        }
    }
    // median to x
    x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
    x->records.insert(x->records.begin() + i, y->records[t - 1]);
    x->children.insert(x->children.begin() + i + 1, z);
    // shrink y
    y->keys.resize(t - 1);
    y->records.resize(t - 1);
    if (!y->leaf) y->children.resize(t);
}

void BTreeBook::insert(const Book& book) {
    if (!root) {
        root = new BTreeBookNode(true);
        root->keys.push_back(book.id);
        root->records.push_back(book);
        return;
    }
    if ((int)root->keys.size() == 2 * t - 1) {
        BTreeBookNode* s = new BTreeBookNode(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    insertNonFull(root, book);
}

void BTreeBook::insertNonFull(BTreeBookNode* x, const Book& book) {
    int i = (int)x->keys.size() - 1;
    if (x->leaf) {
        // maintain sorted order
        x->keys.push_back(""); 
        x->records.push_back(Book{});
        while (i >= 0 && book.id < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->records[i + 1] = x->records[i];
            --i;
        }
        x->keys[i + 1] = book.id;
        x->records[i + 1] = book;
    } else {
        while (i >= 0 && book.id < x->keys[i]) --i;
        ++i;
        if ((int)x->children[i]->keys.size() == 2 * t - 1) {
            splitChild(x, i);
            if (book.id > x->keys[i]) ++i;
        }
        insertNonFull(x->children[i], book);
    }
}

void BTreeBook::inorder(std::vector<Book>& out) const {
    inorderNode(root, out);
}

void BTreeBook::inorderNode(BTreeBookNode* x, std::vector<Book>& out) const {
    if (!x) return;
    for (size_t i = 0; i < x->keys.size(); ++i) {
        if (!x->leaf) inorderNode(x->children[i], out);
        out.push_back(x->records[i]);
    }
    if (!x->leaf) inorderNode(x->children.back(), out);
}

bool BTreeBook::remove(const std::string& id) {
    // rebuild-based removal for simplicity
    std::vector<Book> items;
    inorder(items);
    bool found = false;
    for (auto it = items.begin(); it != items.end(); ) {
        if (it->id == id) { it = items.erase(it); found = true; }
        else ++it;
    }
    destroy(root); root = nullptr;
    for (const auto& b : items) insert(b);
    return found;
}

