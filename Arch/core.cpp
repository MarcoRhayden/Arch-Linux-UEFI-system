#include "lib/arch.h"
using namespace std;

void showMenu() {
    cout << "[Arch Install]" << endl;
    cout << "1 - START INSTALL" << endl;
    cout << "2 - EXIT" << endl;
    cout << "-> ";
}

int main(int argc, char* argv[]) {
    int mode;
    bool check = false;

    while(!check) {
        showMenu();
        if(!(cin >> mode) || (mode != 1 && mode != 2)) {
            cin.clear(); // clear bad input flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard input
            cout << "--> [ERROR] Invalid option" << endl;
        } else {
            check = true;
            if (mode == 1) {
                // ... [OK] Preparing to Install ...
                Arch a;
            }
        }
    }

    return(0);
}
