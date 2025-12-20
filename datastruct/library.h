#include <string>
#include <map>
#include <iostream>
#include <limits>
using namespace std;

struct Book {
    string id;
    string title;
    string author;
    string publisher;
    double price;
    int totalCopies;
    int availableCopies;
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
