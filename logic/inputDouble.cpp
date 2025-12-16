#include "../controller/library.h"


double inputDouble(const string& prompt) {
    while (true) {
        cout << prompt;
        double v;
        if (cin >> v) {
            clearInput();
            return v;
        }
        clearInput();
        cout << "输入无效，请重新输入数字。\n";
    }
}

