//File: Arch.hpp
#ifndef __ARCH_HPP__
#define __ARCH_HPP__

#include <iostream>
#include <algorithm>
#include <limits>
#include <memory>
#include <fstream>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

namespace Arch
{
    class Install
    {
        private:
            // ================================================================================
            // USER SETTINGS
            // ================================================================================
            std::string hostname;
            std::string rootPassword;
            std::string language;
            std::string zone_info;
            std::string keymap;
            // ================================================================================
            // ================================================================================

            bool checkConnection();
            void handle();
            int check_partitions();
            void system_call(const std::string arg);
            bool start();
            bool systemd_setup();

        public:
            Install() {
                this->hostname = "arch"; // YOUR COMPUTER NAME
                this->rootPassword = "ROOTPASSWORD"; // ROOT PASSWORD
                this->language = "en_US.UTF-8"; // FOR PORTUGUESE USE -> "pt_BR.UTF-8"
                this->zone_info = "America/Sao_Paulo"; // ZONE INFO
                this->keymap = "br-abnt2"; // KEYMAP
                handle();
            }

            ~Install() {

            }
    };
}

#endif
