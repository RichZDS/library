#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <limits>
#include <ctime>
using namespace std;

// 借阅记录
struct BorrowRecord {
    string borrowerId;          // 借阅者图书证号
    string borrowerName;        // 借阅者姓名
    time_t borrowTime;          // 借阅时间
    time_t expectedReturnTime;  // 预期归还时间
    time_t actualReturnTime;    // 实际归还时间（0表示未归还）
    BorrowRecord() : borrowTime(0), expectedReturnTime(0), actualReturnTime(0) {}
};

// 预约记录
struct ReservationRecord {
    string reserverId;          // 预约者图书证号
    string reserverName;        // 预约者姓名
    time_t reserveTime;         // 预约时间
    time_t expectedPickupTime;  // 预期取书时间
    time_t actualPickupTime;    // 实际取书时间（0表示未取书）
    ReservationRecord() : reserveTime(0), expectedPickupTime(0), actualPickupTime(0) {}
};

// 图书信息
struct Book {
    string id;              // 书号
    string title;           // 书名
    string author;          // 作者
    string publisher;       // 出版社
    double price;           // 定价
    string publishDate;     // 出版时间
    int totalCopies;        // 总存量
    int availableCopies;    // 现存量
    vector<BorrowRecord> borrowRecords;      // 借阅记录列表
    vector<ReservationRecord> reservationRecords;  // 预约记录列表
    
    Book() : price(0.0), totalCopies(0), availableCopies(0) {}
};

class BTreeBook;
class BTreeAuthor;

void printBook(const Book& book);
void clearInput();
string inputLine(const string& prompt);
int inputInt(const string& prompt);
double inputDouble(const string& prompt);

void addOrAcquireBook(BTreeBook& books, BTreeAuthor& authorIndex);
void removeBook(BTreeBook& books, BTreeAuthor& authorIndex);
void borrowBook(BTreeBook& books);
void returnBook(BTreeBook& books);
void queryById(const BTreeBook& books);
void queryByAuthor(const BTreeBook& books, const BTreeAuthor& authorIndex);
void listAll(const BTreeBook& books);
void showMenu();
