#include "library.h"
#include "btree_book.h"
#include "btree_author.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// 打印图书信息
void printBook(const Book& book) {
    cout << "书号: " << book.id << endl;
    cout << "书名: " << book.title << endl;
    cout << "作者: " << book.author << endl;
    cout << "出版社: " << book.publisher << endl;
    cout << "定价: " << fixed << setprecision(2) << book.price << " 元" << endl;
    cout << "出版时间: " << book.publishDate << endl;
    cout << "总存量: " << book.totalCopies << endl;
    cout << "现存量: " << book.availableCopies << endl;
    
    // 打印借阅记录
    if (!book.borrowRecords.empty()) {
        cout << "\n借阅记录:" << endl;
        for (const auto& record : book.borrowRecords) {
            cout << "  借阅者证号: " << record.borrowerId 
                 << ", 姓名: " << record.borrowerName;
            if (record.borrowTime > 0) {
                char buf[64];
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&record.borrowTime));
                cout << ", 借阅时间: " << buf;
            }
            if (record.actualReturnTime == 0) {
                cout << " [借阅中]";
            } else {
                cout << " [已归还]";
            }
            cout << endl;
        }
    }
    
    // 打印预约记录
    if (!book.reservationRecords.empty()) {
        cout << "\n预约记录:" << endl;
        for (const auto& record : book.reservationRecords) {
            cout << "  预约者证号: " << record.reserverId 
                 << ", 姓名: " << record.reserverName;
            if (record.reserveTime > 0) {
                char buf[64];
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&record.reserveTime));
                cout << ", 预约时间: " << buf;
            }
            if (record.actualPickupTime == 0) {
                cout << " [预约中]";
            } else {
                cout << " [已取书]";
            }
            cout << endl;
        }
    }
}

// 清空输入缓冲区
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 输入一行字符串
string inputLine(const string& prompt) {
    string line;
    cout << prompt;
    getline(cin, line);
    return line;
}

// 输入整数
int inputInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "输入错误，请重新输入: ";
        clearInput();
    }
    clearInput();
    return value;
}

// 输入浮点数
double inputDouble(const string& prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "输入错误，请重新输入: ";
        clearInput();
    }
    clearInput();
    return value;
}

// 采编入库
void addOrAcquireBook(BTreeBook& books, BTreeAuthor& authorIndex) {
    Book book;
    book.id = inputLine("请输入书号: ");
    if (book.id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    
    Book* exist = books.search(book.id);
    if (exist) {
        // 图书已存在，只增加册数
        int copies = inputInt("请输入增加的册数: ");
        if (copies > 0) {
            exist->totalCopies += copies;
            exist->availableCopies += copies;
            cout << "成功增加 " << copies << " 册，当前总存量: " << exist->totalCopies << endl;
        }
    } else {
        // 新书，输入完整信息
        book.title = inputLine("请输入书名: ");
        book.author = inputLine("请输入作者: ");
        book.publisher = inputLine("请输入出版社: ");
        book.price = inputDouble("请输入定价: ");
        book.publishDate = inputLine("请输入出版时间: ");
        book.totalCopies = inputInt("请输入总册数: ");
        book.availableCopies = book.totalCopies;
        
        books.insert(book);
        authorIndex.add(book.author, book.id);
        cout << "新书入库成功！" << endl;
    }
    
    // 显示B树结构
    cout << "\n当前B树结构（凹入表）:" << endl;
    cout << books.getStructure() << endl;
}

// 清除库存
void removeBook(BTreeBook& books, BTreeAuthor& authorIndex) {
    string id = inputLine("请输入要删除的书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    
    Book* book = books.search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    
    string author = book->author;
    if (books.remove(id)) {
        authorIndex.removePair(author, id);
        cout << "删除成功！" << endl;
    } else {
        cout << "删除失败！" << endl;
        return;
    }
    
    // 显示B树结构
    cout << "\n当前B树结构（凹入表）:" << endl;
    cout << books.getStructure() << endl;
}

// 图书借阅
void borrowBook(BTreeBook& books) {
    string id = inputLine("请输入书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    
    Book* book = books.search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    
    if (book->availableCopies <= 0) {
        cout << "该书库存为零，无法借阅！" << endl;
        return;
    }
    
    BorrowRecord record;
    record.borrowerId = inputLine("请输入借阅者图书证号: ");
    record.borrowerName = inputLine("请输入借阅者姓名: ");
    
    if (record.borrowerId.empty() || record.borrowerName.empty()) {
        cout << "借阅者信息不完整！" << endl;
        return;
    }
    
    record.borrowTime = time(nullptr);
    record.expectedReturnTime = record.borrowTime + 30 * 24 * 3600; // 30天后
    record.actualReturnTime = 0;
    
    book->borrowRecords.push_back(record);
    book->availableCopies -= 1;
    
    cout << "借阅成功！" << endl;
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&record.borrowTime));
    cout << "借阅时间: " << buf << endl;
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&record.expectedReturnTime));
    cout << "预期归还时间: " << buf << endl;
}

// 图书归还
void returnBook(BTreeBook& books) {
    string id = inputLine("请输入书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    
    Book* book = books.search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    
    if (book->availableCopies >= book->totalCopies) {
        cout << "该书已全部归还，无需再归还！" << endl;
        return;
    }
    
    string borrowerId = inputLine("请输入借阅者图书证号: ");
    if (borrowerId.empty()) {
        cout << "借阅者证号不能为空！" << endl;
        return;
    }
    
    bool found = false;
    time_t currentTime = time(nullptr);
    
    for (auto& record : book->borrowRecords) {
        if (record.borrowerId == borrowerId && record.actualReturnTime == 0) {
            record.actualReturnTime = currentTime;
            book->availableCopies += 1;
            found = true;
            break;
        }
    }
    
    if (found) {
        cout << "归还成功！" << endl;
    } else {
        cout << "未找到该借阅者的未归还记录！" << endl;
    }
}

void reserveBook(BTreeBook& books) {
    string id = inputLine("请输入书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    Book* book = books.search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    string reserverId = inputLine("请输入预约者图书证号: ");
    string reserverName = inputLine("请输入预约者姓名: ");
    if (reserverId.empty() || reserverName.empty()) {
        cout << "预约者信息不完整！" << endl;
        return;
    }
    time_t now = time(nullptr);
    ReservationRecord record;
    record.reserverId = reserverId;
    record.reserverName = reserverName;
    record.reserveTime = now;
    record.expectedPickupTime = now + 30 * 24 * 3600;
    record.actualPickupTime = 0;
    book->reservationRecords.push_back(record);
    cout << "预约成功！" << endl;
}

// 按书号查询
void queryById(const BTreeBook& books) {
    string id = inputLine("请输入书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    
    Book* book = books.search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    
    cout << "\n=== 图书信息 ===" << endl;
    printBook(*book);
}

// 按作者查询
void queryByAuthor(const BTreeBook& books, const BTreeAuthor& authorIndex) {
    string author = inputLine("请输入作者姓名: ");
    if (author.empty()) {
        cout << "作者姓名不能为空！" << endl;
        return;
    }
    
    vector<string> bookIds = authorIndex.getIds(author);
    if (bookIds.empty()) {
        cout << "未找到该作者的图书！" << endl;
        return;
    }
    
    cout << "\n=== 作者专区： " << author << " ===" << endl;
    cout << "共找到 " << bookIds.size() << " 本图书：" << endl;
    cout << endl;
    
    for (const auto& id : bookIds) {
        Book* book = const_cast<BTreeBook&>(books).search(id);
        if (book) {
            printBook(*book);
            cout << "\n" << string(50, '-') << "\n" << endl;
        }
    }
}

// 列出所有图书
void listAll(const BTreeBook& books) {
    vector<Book> allBooks;
    books.inorder(allBooks);
    
    if (allBooks.empty()) {
        cout << "图书库为空！" << endl;
        return;
    }
    
    cout << "\n=== 所有图书信息 ===" << endl;
    cout << "共 " << allBooks.size() << " 本图书：" << endl;
    cout << endl;
    
    for (const auto& book : allBooks) {
        printBook(book);
        cout << "\n" << string(50, '-') << "\n" << endl;
    }
}

void listAllBorrowers(const BTreeBook& books) {
    vector<Book> allBooks;
    books.inorder(allBooks);
    bool any = false;
    for (const auto& book : allBooks) {
        for (const auto& r : book.borrowRecords) {
            any = true;
            cout << "书号: " << book.id << " | 借阅者证号: " << r.borrowerId << " | 姓名: " << r.borrowerName;
            if (r.borrowTime > 0) {
                char buf[64];
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&r.borrowTime));
                cout << " | 借阅时间: " << buf;
            }
            cout << endl;
        }
    }
    if (!any) cout << "当前暂无借阅记录。" << endl;
}

void listAllReservers(const BTreeBook& books) {
    vector<Book> allBooks;
    books.inorder(allBooks);
    bool any = false;
    for (const auto& book : allBooks) {
        for (const auto& r : book.reservationRecords) {
            any = true;
            cout << "书号: " << book.id << " | 预约者证号: " << r.reserverId << " | 姓名: " << r.reserverName;
            if (r.reserveTime > 0) {
                char buf[64];
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&r.reserveTime));
                cout << " | 预约时间: " << buf;
            }
            cout << endl;
        }
    }
    if (!any) cout << "当前暂无预约记录。" << endl;
}

void viewBorrowReserveById(const BTreeBook& books) {
    string id = inputLine("请输入书号: ");
    if (id.empty()) {
        cout << "书号不能为空！" << endl;
        return;
    }
    Book* book = const_cast<BTreeBook&>(books).search(id);
    if (!book) {
        cout << "未找到该书！" << endl;
        return;
    }
    cout << "\n=== 图书信息 ===" << endl;
    printBook(*book);
}

// 显示菜单
void showMenu() {
    cout << "\n========================================" << endl;
    cout << "        图书管理系统" << endl;
    cout << "========================================" << endl;
    cout << "1. 采编入库" << endl;
    cout << "2. 清除库存" << endl;
    cout << "3. 图书借阅" << endl;
    cout << "4. 图书归还" << endl;
    cout << "5. 按书号查询" << endl;
    cout << "6. 作者专区" << endl;
    cout << "7. 图书信息（全部）" << endl;
    cout << "0. 退出" << endl;
    cout << "========================================" << endl;
    cout << "请选择操作: ";
}

// ========== 非交互式函数实现（供API/Web使用） ==========

// 添加或更新图书（非交互式）
bool addOrAcquireBook(BTreeBook& books, BTreeAuthor& authorIndex, const Book& book) {
    if (book.id.empty()) {
        return false;
    }
    
    Book* exist = books.search(book.id);
    if (exist) {
        // 图书已存在，只增加册数
        exist->totalCopies += book.totalCopies;
        exist->availableCopies += book.totalCopies;
        return true;
    } else {
        // 新书，插入B树
        books.insert(book);
        authorIndex.add(book.author, book.id);
        return true;
    }
}

// 根据ID删除图书（非交互式）
bool removeBookById(BTreeBook& books, BTreeAuthor& authorIndex, const string& id) {
    if (id.empty()) {
        return false;
    }
    
    Book* book = books.search(id);
    if (!book) {
        return false;
    }
    
    string author = book->author;
    if (books.remove(id)) {
        authorIndex.removePair(author, id);
        return true;
    }
    return false;
}

// 根据ID借阅图书（非交互式）
bool borrowBookById(BTreeBook& books, const string& id, const string& borrowerId, const string& borrowerName) {
    if (id.empty() || borrowerId.empty() || borrowerName.empty()) {
        return false;
    }
    
    Book* book = books.search(id);
    if (!book) {
        return false;
    }
    
    if (book->availableCopies <= 0) {
        return false;
    }
    
    BorrowRecord record;
    record.borrowerId = borrowerId;
    record.borrowerName = borrowerName;
    record.borrowTime = time(nullptr);
    record.expectedReturnTime = record.borrowTime + 30 * 24 * 3600; // 30天后
    record.actualReturnTime = 0;
    
    book->borrowRecords.push_back(record);
    book->availableCopies -= 1;
    
    return true;
}

// 根据ID归还图书（非交互式）
bool returnBookById(BTreeBook& books, const string& id, const string& borrowerId) {
    if (id.empty() || borrowerId.empty()) {
        return false;
    }
    
    Book* book = books.search(id);
    if (!book) {
        return false;
    }
    
    if (book->availableCopies >= book->totalCopies) {
        return false;
    }
    
    bool found = false;
    time_t currentTime = time(nullptr);
    
    for (auto& record : book->borrowRecords) {
        if (record.borrowerId == borrowerId && record.actualReturnTime == 0) {
            record.actualReturnTime = currentTime;
            book->availableCopies += 1;
            found = true;
            break;
        }
    }
    
    return found;
}

void queryByTitle(const BTreeBook& books) {
    string title = inputLine("请输入书名: ");
    if (title.empty()) {
        cout << "书名不能为空！" << endl;
        return;
    }
    vector<Book> all;
    books.inorder(all);
    vector<Book> result;
    for (const auto& b : all) {
        if (b.title == title) result.push_back(b);
    }
    if (result.empty()) {
        cout << "未找到该书名的图书！" << endl;
        return;
    }
    cout << "\n=== 书名: " << title << " 的图书 ===" << endl;
    for (const auto& b : result) {
        printBook(b);
        cout << "\n" << string(50, '-') << "\n" << endl;
    }
}

vector<Book> queryBooksByTitle(const BTreeBook& books, const string& title) {
    vector<Book> result;
    vector<Book> all;
    books.inorder(all);
    for (const auto& b : all) {
        if (b.title == title) result.push_back(b);
    }
    return result;
}
// 根据ID预约图书（非交互式）
bool reserveBookById(BTreeBook& books, const string& id, const string& reserverId, const string& reserverName) {
    if (id.empty() || reserverId.empty() || reserverName.empty()) {
        return false;
    }
    
    Book* book = books.search(id);
    if (!book) {
        return false;
    }
    
    ReservationRecord record;
    record.reserverId = reserverId;
    record.reserverName = reserverName;
    record.reserveTime = time(nullptr);
    record.expectedPickupTime = record.reserveTime + 30 * 24 * 3600; // 30天后
    record.actualPickupTime = 0;
    
    book->reservationRecords.push_back(record);
    return true;
}

// 根据ID查询图书（非交互式）
Book* queryBookById(const BTreeBook& books, const string& id) {
    if (id.empty()) {
        return nullptr;
    }
    return const_cast<BTreeBook&>(books).search(id);
}

// 根据作者查询图书（非交互式）
vector<Book> queryBooksByAuthor(const BTreeBook& books, const BTreeAuthor& authorIndex, const string& author) {
    vector<Book> result;
    if (author.empty()) {
        return result;
    }
    
    vector<string> bookIds = authorIndex.getIds(author);
    for (const auto& id : bookIds) {
        Book* book = const_cast<BTreeBook&>(books).search(id);
        if (book) {
            result.push_back(*book);
        }
    }
    return result;
}

// 获取所有图书（非交互式）
vector<Book> getAllBooks(const BTreeBook& books) {
    vector<Book> result;
    books.inorder(result);
    return result;
}

// 获取B树结构（非交互式）
string getTreeStructure(const BTreeBook& books) {
    return books.getStructure();
}

