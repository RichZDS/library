#pragma once
#include <string>
#include <vector>
#include "controller/library.h"

struct BTreeBookNode {
    bool leaf;
    std::vector<std::string> keys;           // book.id
    std::vector<Book> records;               // same size as keys
    std::vector<BTreeBookNode*> children;    // size = keys.size() + 1
    BTreeBookNode(bool leaf): leaf(leaf) {}
};

class BTreeBook {
public:
    explicit BTreeBook(int t = 2);
    ~BTreeBook();
    Book* search(const std::string& id) const;
    void insert(const Book& book);
    void inorder(std::vector<Book>& out) const;
    bool remove(const std::string& id); // rebuild-based removal
    std::string getStructure() const;
private:
    int t; // minimum degree
    BTreeBookNode* root;
    void destroy(BTreeBookNode* x);
    Book* searchNode(BTreeBookNode* x, const std::string& id) const;
    void splitChild(BTreeBookNode* x, int i);
    void insertNonFull(BTreeBookNode* x, const Book& book);
    void inorderNode(BTreeBookNode* x, std::vector<Book>& out) const;
    void getStructureNode(BTreeBookNode* x, int depth, std::stringstream& ss) const;
};

