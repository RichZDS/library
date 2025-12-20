#pragma once
#include "datastruct/library.h"
#include "datastruct/btree_book.h"
#include "datastruct/btree_author.h"
#include <vector>
#include <string>

using namespace std;

class BookService {
private:
  BTreeBook m_books;
  BTreeAuthor m_authors;

public:
  // 添加或更新图书
  void addOrAcquire(const Book& b) {
    Book* exist = m_books.search(b.id);
    if (exist) {
      // 图书已存在，增加册数
      exist->totalCopies += b.totalCopies;
      exist->availableCopies += b.availableCopies;
    } else {
      // 新图书，插入B树
      m_books.insert(b);
      m_authors.add(b.author, b.id);
    }
  }

  // 根据ID删除图书
  bool removeById(const string& id) {
    Book* rec = m_books.search(id);
    if (!rec) return false;
    m_authors.removePair(rec->author, id);
    return m_books.remove(id);
  }

  // 根据ID获取图书
  Book* getById(const string& id) {
    return m_books.search(id);
  }

  // 获取所有图书列表
  vector<Book> listAll() {
    vector<Book> out;
    m_books.inorder(out);
    return out;
  }

  // 借阅图书
  bool borrow(const string& id) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies <= 0) return false;
    rec->availableCopies -= 1;
    return true;
  }

  // 归还图书
  bool giveback(const string& id) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies >= rec->totalCopies) return false;
    rec->availableCopies += 1;
    return true;
  }

  // 获取B树结构（凹入表）
  string getTreeStructure() const {
    return m_books.getStructure();
  }
};

