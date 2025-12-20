#pragma once
#include "controller/library.h"
#include "datastruct/btree_book.h"
#include "datastruct/btree_author.h"
#include <vector>

class BookService {
private:
  BTreeBook m_books;
  BTreeAuthor m_authors;
public:
  void addOrAcquire(const Book& b) {
    Book* exist = m_books.search(b.id);
    if (exist) {
      exist->totalCopies += b.totalCopies;
      exist->availableCopies += b.availableCopies;
    } else {
      m_books.insert(b);
      m_authors.add(b.author, b.id);
    }
  }
  bool removeById(const std::string& id) {
    Book* rec = m_books.search(id);
    if (!rec) return false;
    m_authors.removePair(rec->author, id);
    return m_books.remove(id);
  }
  Book* getById(const std::string& id) {
    return m_books.search(id);
  }
  std::vector<Book> listAll() {
    std::vector<Book> out;
    m_books.inorder(out);
    return out;
  }
  bool borrow(const std::string& id) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies <= 0) return false;
    rec->availableCopies -= 1;
    return true;
  }
  bool giveback(const std::string& id) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies >= rec->totalCopies) return false;
    rec->availableCopies += 1;
    return true;
  }
  std::string getTreeStructure() const {
    return m_books.getStructure();
  }
};

