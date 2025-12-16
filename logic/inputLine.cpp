#include "../controller/library.h"


string inputLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

