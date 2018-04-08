//File: Core.cpp
#include "lib/Arch.hpp"

void showMenu()
{
    std::cout << "[Arch Install]" << std::endl;
    std::cout << "1 - START INSTALL" << std::endl;
    std::cout << "2 - EXIT" << std::endl;
    std::cout << "-> ";
}

int main(int argc, char* argv[])
{
    int mode;
    bool check = false;

    while (!check)
    {
        showMenu();
        if(!(std::cin >> mode) || (mode != 1 && mode != 2))
        {
            std::cin.clear(); // clear bad input flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard input
            std::cout << "--> [ERROR] Invalid option" << std::endl;
        }
        else
        {
            check = true;
            if (mode == 1)
            {
                // ... [OK] Preparing to Install ...
                Arch::Install ArchObj;
            }
        }
    }

    return(0);
}
