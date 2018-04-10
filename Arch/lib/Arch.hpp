//File: Arch.hpp
#ifndef __ARCH_HPP__
#define __ARCH_HPP__

#include <iostream>
#include <readline/readline.h>
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
            std::string hostname;
            std::string username;
            std::string userPassword;
            std::string rootPassword;
            std::string language;
            std::string zone_info;
            std::string keymap;
            std::string video_drivers;
            std::string user_packages;

            bool state; // installation flag
            bool isMounted; // partitions flag

            bool check_connection();
            int check_partitions();
            void system_call(const std::string arg);
            void start();
            bool systemd_setup();

            void GUI_install();
            bool XFCE_install();

        public:
            Install()
            {
                // ================================================================================
                // USER SETTINGS
                // ================================================================================
                this->hostname = "arch"; // YOUR COMPUTER NAME
                this->username = "rhayden"; // YOUR USER NAME
                this->userPassword = "USERPASSWORD"; // YOUR USER NAME
                this->rootPassword = "ROOTPASSWORD"; // ROOT PASSWORD
                this->language = "en_US.UTF-8"; // FOR PORTUGUESE USE -> "pt_BR.UTF-8"
                this->zone_info = "America/Sao_Paulo"; // ZONE INFO
                this->keymap = "br-abnt2"; // KEYMAP

                this->video_drivers = "xf86-video-nouveau mesa xf86-video-intel"; // YOUR VIDEO DRIVERS

                // YOUR FAVORITE PACKAGES
                this->user_packages = "wireless_tools wpa_supplicant wpa_actiond bluez bluez-utils pulseaudio-bluetooth blueman"
                " firefox ttf-dejavu p7zip zip unzip tumbler libmpeg2 ffmpeg ffmpegthumbnailer gvfs gvfs-smb networkmanager"
                " network-manager-applet dhclient xfce4-notifyd gnome-keyring"
                " pavucontrol xarchiver git sqlitebrowser vlc qt4 xpdf galculator ntfs-3g boost openssl";
                // ================================================================================
                // ================================================================================

                this->state = false;
                this->isMounted = false;
            }

            ~Install()
            {
                if (this->isMounted)
                {
                    system_call("umount -R /mnt/boot");
                    system_call("umount -R /mnt");
                }
            }

            void run();
    };
}

#endif
