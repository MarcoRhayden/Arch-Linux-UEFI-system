#ifndef __ARCH_H__
#define __ARCH_H__

#include <iostream>
#include <algorithm>
#include <limits>
#include <memory>
#include <fstream>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

class Arch {
    public:
        Arch();
        ~Arch();

    private:
        // ================================================================================
        // USER SETTINGS
        // ================================================================================
        const std::string hostname = "arch"; // YOUR COMPUTER NAME
        const std::string rootPassword = "ROOTPASSWORD"; // ROOT PASSWORD
        const std::string language = "en_US.UTF-8"; // FOR PORTUGUESE USE -> "pt_BR.UTF-8"
        const std::string zone_info = "America/Sao_Paulo"; // ZONE INFO
        // ================================================================================
        // ================================================================================

        bool checkConnection();
        int check_partitions();
        void system_call(const std::string arg);
        bool ArchInstall();
        bool systemd_setup();
};

#endif
