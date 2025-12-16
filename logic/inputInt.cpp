#include "../controller/library.h"


int inputInt(const string& prompt) {
    while (true) {
        cout << prompt;
        int v;
        if (cin >> v) {
            clearInput();
            return v;
        }
        clearInput();
        cout << "输入无效，请重新输入整数。\n";
    }
}

