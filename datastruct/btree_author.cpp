#include "btree_author.h"
#include <algorithm>

BTreeAuthor::BTreeAuthor(int t): t(t), root(nullptr) {}
BTreeAuthor::~BTreeAuthor() { destroy(root); }

void BTreeAuthor::destroy(BTreeAuthorNode* x) {
    if (!x) return;
    for (auto c : x->children) destroy(c);
    delete x;
}

int BTreeAuthor::findKeyIndex(BTreeAuthorNode* x, const std::string& key) const {
    int i = 0;
    while (i < (int)x->keys.size() && key > x->keys[i]) ++i;
    return i;
}

BTreeAuthorNode* BTreeAuthor::searchNode(BTreeAuthorNode* x, const std::string& author, int& idx) const {
    if (!x) return nullptr;
    idx = findKeyIndex(x, author);
    if (idx < (int)x->keys.size() && x->keys[idx] == author) return x;
    if (x->leaf) return nullptr;
    return searchNode(x->children[idx], author, idx);
}

void BTreeAuthor::splitChild(BTreeAuthorNode* x, int i) {
    BTreeAuthorNode* y = x->children[i];
    BTreeAuthorNode* z = new BTreeAuthorNode(y->leaf);
    for (int j = 0; j < t - 1; ++j) {
        z->keys.push_back(y->keys[j + t]);
        z->ids.push_back(y->ids[j + t]);
    }
    if (!y->leaf) {
        for (int j = 0; j < t; ++j) z->children.push_back(y->children[j + t]);
    }
    x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
    x->ids.insert(x->ids.begin() + i, y->ids[t - 1]);
    x->children.insert(x->children.begin() + i + 1, z);
    y->keys.resize(t - 1);
    y->ids.resize(t - 1);
    if (!y->leaf) y->children.resize(t);
}

void BTreeAuthor::add(const std::string& author, const std::string& id) {
    if (!root) {
        root = new BTreeAuthorNode(true);
        root->keys.push_back(author);
        root->ids.push_back(std::vector<std::string>{id});
        return;
    }
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (node) {
        auto& vec = node->ids[idx];
        if (std::find(vec.begin(), vec.end(), id) == vec.end()) vec.push_back(id);
        return;
    }
    if ((int)root->keys.size() == 2 * t - 1) {
        BTreeAuthorNode* s = new BTreeAuthorNode(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    insertNonFull(root, author, id);
}

void BTreeAuthor::insertNonFull(BTreeAuthorNode* x, const std::string& author, const std::string& id) {
    int i = (int)x->keys.size() - 1;
    if (x->leaf) {
        x->keys.push_back("");
        x->ids.push_back(std::vector<std::string>{});
        while (i >= 0 && author < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->ids[i + 1] = x->ids[i];
            --i;
        }
        x->keys[i + 1] = author;
        x->ids[i + 1] = std::vector<std::string>{id};
    } else {
        while (i >= 0 && author < x->keys[i]) --i;
        ++i;
        if ((int)x->children[i]->keys.size() == 2 * t - 1) {
            splitChild(x, i);
            if (author > x->keys[i]) ++i;
        }
        insertNonFull(x->children[i], author, id);
    }
}

void BTreeAuthor::removePair(const std::string& author, const std::string& id) {
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (!node) return;
    auto& vec = node->ids[idx];
    vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
    if (vec.empty()) {
        // rebuild remove author
        std::vector<std::pair<std::string, std::vector<std::string>>> items;
        inorderAuthors(root, items);
        items.erase(std::remove_if(items.begin(), items.end(), [&](auto& p){ return p.first == author; }), items.end());
        destroy(root); root = nullptr;
        for (auto& p : items) for (auto& bid : p.second) add(p.first, bid);
    }
}

void BTreeAuthor::inorderAuthors(BTreeAuthorNode* x, std::vector<std::pair<std::string, std::vector<std::string>>>& out) const {
    if (!x) return;
    for (size_t i = 0; i < x->keys.size(); ++i) {
        if (!x->leaf) inorderAuthors(x->children[i], out);
        out.push_back({x->keys[i], x->ids[i]});
    }
    if (!x->leaf) inorderAuthors(x->children.back(), out);
}

std::vector<std::string> BTreeAuthor::getIds(const std::string& author) const {
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (!node) return {};
    return node->ids[idx];
}

