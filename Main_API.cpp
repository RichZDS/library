#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "./controller/library.h"
#include "./datastruct/btree_book.h"
#include "./datastruct/btree_author.h"

// 全局变量：用于API模式下的数据存储
static BTreeBook* g_books = nullptr;
static BTreeAuthor* g_authorIndex = nullptr;

// 初始化全局数据结构
void initData() {
    if (!g_books) {
        g_books = new BTreeBook(2);
        g_authorIndex = new BTreeAuthor(2);
    }
}

// 清理全局数据
void cleanupData() {
    if (g_books) {
        delete g_books;
        g_books = nullptr;
    }
    if (g_authorIndex) {
        delete g_authorIndex;
        g_authorIndex = nullptr;
    }
}

// 将字符串转义为JSON格式
std::string jsonEscape(const std::string& str) {
    std::ostringstream o;
    for (size_t i = 0; i < str.length(); ++i) {
        switch (str[i]) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default: o << str[i]; break;
        }
    }
    return o.str();
}

// 将Book对象转换为JSON
std::string bookToJson(const Book& book) {
    std::ostringstream json;
    json << "{"
         << "\"id\":\"" << jsonEscape(book.id) << "\","
         << "\"title\":\"" << jsonEscape(book.title) << "\","
         << "\"author\":\"" << jsonEscape(book.author) << "\","
         << "\"publisher\":\"" << jsonEscape(book.publisher) << "\","
         << "\"price\":" << std::fixed << std::setprecision(2) << book.price << ","
         << "\"totalCopies\":" << book.totalCopies << ","
         << "\"availableCopies\":" << book.availableCopies
         << "}";
    return json.str();
}

// API: 添加图书
void apiAddBook(const std::string& id, const std::string& title, const std::string& author,
                const std::string& publisher, double price, int totalCopies) {
    initData();
    
    Book* exist = g_books->search(id);
    if (exist) {
        // 图书已存在，增加册数
        exist->totalCopies += totalCopies;
        exist->availableCopies += totalCopies;
        std::cout << "{\"success\":true,\"message\":\"图书册数已更新\",\"book\":" 
                  << bookToJson(*exist) << "}" << std::endl;
        return;
    }
    
    // 新图书
    Book book;
    book.id = id;
    book.title = title;
    book.author = author;
    book.publisher = publisher;
    book.price = price;
    book.totalCopies = totalCopies;
    book.availableCopies = totalCopies;
    
    g_books->insert(book);
    g_authorIndex->add(author, id);
    
    std::cout << "{\"success\":true,\"message\":\"新书已入库\",\"book\":" 
              << bookToJson(book) << "}" << std::endl;
}

// API: 删除图书
void apiRemoveBook(const std::string& id) {
    initData();
    
    Book* book = g_books->search(id);
    if (!book) {
        std::cout << "{\"success\":false,\"message\":\"未找到该图书\"}" << std::endl;
        return;
    }
    
    std::string author = book->author;
    bool removed = g_books->remove(id);
    if (removed) {
        g_authorIndex->removePair(author, id);
        std::cout << "{\"success\":true,\"message\":\"图书已删除\"}" << std::endl;
    } else {
        std::cout << "{\"success\":false,\"message\":\"删除失败\"}" << std::endl;
    }
}

// API: 借阅图书
void apiBorrowBook(const std::string& id) {
    initData();
    
    Book* book = g_books->search(id);
    if (!book) {
        std::cout << "{\"success\":false,\"message\":\"未找到该图书\"}" << std::endl;
        return;
    }
    
    if (book->availableCopies <= 0) {
        std::cout << "{\"success\":false,\"message\":\"库存不足，无法借阅\",\"book\":" 
                  << bookToJson(*book) << "}" << std::endl;
        return;
    }
    
    book->availableCopies -= 1;
    std::cout << "{\"success\":true,\"message\":\"借阅成功\",\"book\":" 
              << bookToJson(*book) << "}" << std::endl;
}

// API: 归还图书
void apiReturnBook(const std::string& id) {
    initData();
    
    Book* book = g_books->search(id);
    if (!book) {
        std::cout << "{\"success\":false,\"message\":\"未找到该图书\"}" << std::endl;
        return;
    }
    
    if (book->availableCopies >= book->totalCopies) {
        std::cout << "{\"success\":false,\"message\":\"该图书已全部归还\"}" << std::endl;
        return;
    }
    
    book->availableCopies += 1;
    std::cout << "{\"success\":true,\"message\":\"归还成功\",\"book\":" 
              << bookToJson(*book) << "}" << std::endl;
}

// API: 根据ID查询图书
void apiQueryById(const std::string& id) {
    initData();
    
    Book* book = g_books->search(id);
    if (!book) {
        std::cout << "{\"success\":false,\"message\":\"未找到该图书\"}" << std::endl;
        return;
    }
    
    std::cout << "{\"success\":true,\"book\":" << bookToJson(*book) << "}" << std::endl;
}

// API: 根据作者查询图书
void apiQueryByAuthor(const std::string& author) {
    initData();
    
    std::vector<std::string> ids = g_authorIndex->getIds(author);
    if (ids.empty()) {
        std::cout << "{\"success\":false,\"message\":\"未找到该作者的图书\",\"books\":[]}" << std::endl;
        return;
    }
    
    std::ostringstream json;
    json << "{\"success\":true,\"author\":\"" << jsonEscape(author) << "\",\"books\":[";
    
    bool first = true;
    for (const auto& id : ids) {
        Book* book = g_books->search(id);
        if (book) {
            if (!first) json << ",";
            json << bookToJson(*book);
            first = false;
        }
    }
    
    json << "]}";
    std::cout << json.str() << std::endl;
}

// API: 列出所有图书
void apiListAll() {
    initData();
    
    std::vector<Book> items;
    g_books->inorder(items);
    
    std::ostringstream json;
    json << "{\"success\":true,\"books\":[";
    
    bool first = true;
    for (const auto& book : items) {
        if (!first) json << ",";
        json << bookToJson(book);
        first = false;
    }
    
    json << "]}";
    std::cout << json.str() << std::endl;
}

// 解析命令行参数并调用相应的API
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    if (argc < 2) {
        std::cout << "{\"success\":false,\"message\":\"缺少操作参数\"}" << std::endl;
        return 1;
    }
    
    std::string action = argv[1];
    
    try {
        if (action == "add") {
            if (argc < 8) {
                std::cout << "{\"success\":false,\"message\":\"参数不足\"}" << std::endl;
                return 1;
            }
            apiAddBook(argv[2], argv[3], argv[4], argv[5], 
                      std::stod(argv[6]), std::stoi(argv[7]));
        }
        else if (action == "remove") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"message\":\"缺少图书编号\"}" << std::endl;
                return 1;
            }
            apiRemoveBook(argv[2]);
        }
        else if (action == "borrow") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"message\":\"缺少图书编号\"}" << std::endl;
                return 1;
            }
            apiBorrowBook(argv[2]);
        }
        else if (action == "return") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"message\":\"缺少图书编号\"}" << std::endl;
                return 1;
            }
            apiReturnBook(argv[2]);
        }
        else if (action == "query") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"message\":\"缺少图书编号\"}" << std::endl;
                return 1;
            }
            apiQueryById(argv[2]);
        }
        else if (action == "queryAuthor") {
            if (argc < 3) {
                std::cout << "{\"success\":false,\"message\":\"缺少作者名\"}" << std::endl;
                return 1;
            }
            apiQueryByAuthor(argv[2]);
        }
        else if (action == "list") {
            apiListAll();
        }
        else {
            std::cout << "{\"success\":false,\"message\":\"未知操作: " << jsonEscape(action) << "\"}" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cout << "{\"success\":false,\"message\":\"错误: " << jsonEscape(e.what()) << "\"}" << std::endl;
        return 1;
    }
    
    return 0;
}

