#pragma once
#include "library_core/library.h"
#include "library_core/btree_book.h"
#include "library_core/btree_author.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

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
      exist->availableCopies += b.totalCopies;  // 新增的册数都是可借的
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

  // 借阅图书（记录借阅者信息）
  bool borrow(const string& id, const string& borrowerId, const string& borrowerName) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies <= 0) return false;
    
    // 创建借阅记录
    BorrowRecord record;
    record.borrowerId = borrowerId;
    record.borrowerName = borrowerName;
    record.borrowTime = time(nullptr);  // 当前时间
    
    // 计算预期归还时间（假设借期为30天）
    record.expectedReturnTime = record.borrowTime + 30 * 24 * 3600;
    record.actualReturnTime = 0;  // 未归还
    
    // 添加借阅记录
    rec->borrowRecords.push_back(record);
    
    // 减少可借数量
    rec->availableCopies -= 1;
    
    return true;
  }

  // 归还图书（注销借阅者信息）
  bool giveback(const string& id, const string& borrowerId) {
    auto rec = m_books.search(id);
    if (!rec || rec->availableCopies >= rec->totalCopies) return false;
    
    // 查找未归还的借阅记录（actualReturnTime为0表示未归还）
    bool found = false;
    time_t currentTime = time(nullptr);
    
    for (auto& record : rec->borrowRecords) {
      // 检查是否为该借阅者的未归还记录
      if (record.borrowerId == borrowerId && record.actualReturnTime == 0) {
        // 标记为已归还
        record.actualReturnTime = currentTime;
        found = true;
        break;
      }
    }
    
    if (found) {
      rec->availableCopies += 1;
      return true;
    }
    
    return false;
  }

  // 预约图书（当库存为零时）
  bool reserve(const string& id, const string& reserverId, const string& reserverName) {
    auto rec = m_books.search(id);
    if (!rec) return false;
    
    // 只有当库存为零时才能预约
    if (rec->availableCopies > 0) return false;
    
    // 检查是否已经预约过（同一个证号，且未取书）
    for (const auto& record : rec->reservationRecords) {
      if (record.reserverId == reserverId && record.actualPickupTime == 0) {
        // 已经存在未取书的预约记录
        return false;
      }
    }
    
    // 创建预约记录
    ReservationRecord record;
    record.reserverId = reserverId;
    record.reserverName = reserverName;
    record.reserveTime = time(nullptr);  // 当前时间
    
    // 计算预期取书时间（假设预约保留期为7天）
    record.expectedPickupTime = record.reserveTime + 7 * 24 * 3600;
    record.actualPickupTime = 0;  // 未取书
    
    // 添加预约记录
    rec->reservationRecords.push_back(record);
    
    return true;
  }

  // 根据作者名获取图书列表
  vector<Book> getByAuthor(const string& author) {
    vector<Book> result;
    vector<string> bookIds = m_authors.getIds(author);
    
    for (const auto& id : bookIds) {
      Book* book = m_books.search(id);
      if (book) {
        result.push_back(*book);
      }
    }
    
    return result;
  }

  // 获取B树结构（凹入表）
  string getTreeStructure() const {
    return m_books.getStructure();
  }
};

