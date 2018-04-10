//File: Arch.cpp
#include "../lib/Arch.hpp"
/*---------------------------------------------------------------------*/
/*--- run - Start arch installation                                 ---*/
/*---------------------------------------------------------------------*/
void Arch::Install::run()
{
    if (Arch::Install::check_connection() != 0)
    {
        std::cout << "--> [ERROR] No internet connection..." << std::endl;
        std::cout << "--> [ERROR] Installation aborted..." << std::endl;
    }
    else if (Arch::Install::check_partitions() != 0)
    {
        std::cout << "--> [ERROR] Wrong partitions table..." << std::endl;
        std::cout << "--> The partitions configuration must be: "
            << "sda1(boot)[EF00] 500MB, sda2(swap)[8200] ?G, "
            << "sda3(arch)[8300] ?G" << std::endl;
    }
    else
    {
        // ----------------------------------------------------------------
        // ... [OK] Start Installation ...
        Arch::Install::system_call("mkfs.vfat /dev/sda1"); // formating boot partition
        Arch::Install::system_call("mkswap /dev/sda2"); // making swap on sda2
        Arch::Install::system_call("swapon /dev/sda2"); // swapon on sda2
        Arch::Install::system_call("mkfs.ext4 /dev/sda3"); // format arch partition
        Arch::Install::system_call("timedatectl set-ntp true"); // update clock
        Arch::Install::system_call("pacman -Syy"); // synchronizing package

        Arch::Install::start(); // Start Installation

        if (this->state)
        {
            // ... [OK] Installation Successfully ...
            std::cout << "\n--> [OK] Arch Linux successfully installed!" << std::endl;
            Arch::Install::GUI_install();
            std::cout << "\n--> [OK] All Done!" << std::endl;
        }
        else
        {
            // ... [X] Installation Failed ...
            std::cout << "\n--> [X] Arch Linux installation failed." << std::endl;
        }
        // ----------------------------------------------------------------
    }
}
/*---------------------------------------------------------------------*/
/*--- system_call - Requests a core system service                  ---*/
/*---------------------------------------------------------------------*/
void Arch::Install::system_call(const std::string arg)
{
    int status = system(arg.c_str());
    if (status != 0)
    {
        std::cout << "--> [ERROR] System call failed..." << std::endl;
        std::cout << "--> [ERROR] Installation aborted..." << std::endl;
        exit(1);
    }
}
/*---------------------------------------------------------------------*/
/*--- check_connection - Check network connection                   ---*/
/*---------------------------------------------------------------------*/
bool Arch::Install::check_connection()
{
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    return(getaddrinfo("www.google.com", "80", &host_info, &host_info_list));
}
/*---------------------------------------------------------------------*/
/*--- check_partitions - Check the partition table to see if it     ---*/
/*--- is in the correct software standard                           ---*/
/*---------------------------------------------------------------------*/
int Arch::Install::check_partitions()
{
    int status;
    const char sda_boot[] = "/dev/sda1"; // EFI PARTITION
    const char sda_swap[] = "/dev/sda2"; // Swap PARTITION
    const char sda_arch[] = "/dev/sda3"; // Arch PARTITION

    if((status = open(sda_boot, O_RDONLY | O_SYNC) == -1)) {
        return(1);
    }

    if((status = open(sda_swap, O_RDONLY | O_SYNC) == -1)) {
        return(1);
    }

    if((status = open(sda_arch, O_RDONLY | O_SYNC) == -1)) {
        return(1);
    }

    return(0);
}
/*---------------------------------------------------------------------*/
/*--- start - Start Arch Linux installation                         ---*/
/*---------------------------------------------------------------------*/
void Arch::Install::start()
{
    // ----------------------------------------------------------------
    Arch::Install::system_call("mount /dev/sda3 /mnt");
    Arch::Install::system_call("mkdir /mnt/boot");
    Arch::Install::system_call("mount /dev/sda1 /mnt/boot");
    this->isMounted = true;
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    Arch::Install::system_call("pacstrap -i /mnt base base-devel"); // install arch base
    Arch::Install::system_call("genfstab -U -p /mnt >> /mnt/etc/fstab"); // generate fstab file
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/"+this->zone_info+" /etc/localtime\""); // timezone
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"hwclock --systohc --utc\""); // generate /etc/adjtime
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"echo "+this->language+" UTF-8 > /etc/locale.gen\""); // locale config
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"locale-gen\""); // locale config
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"echo LANG="+this->language+" > /etc/locale.conf\""); // locale config
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"export LANG="+this->language+"\""); // locale config
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"echo "+this->hostname+" > /etc/hostname\""); // hostname
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"systemctl enable dhcpcd\""); // network config
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"echo \"root:"+this->rootPassword+"\" | sudo chpasswd\""); // setting root password

    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"useradd -m -g users -G wheel -s /bin/bash "+this->username+"\""); // adding user
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"echo \""+this->username+":"+this->userPassword+"\" | sudo chpasswd\""); // setting user password
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"pacman -S "+this->user_packages+"\""); // install user packages

    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""); // setting NetworkManager
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"systemctl start NetworkManager\""); // setting NetworkManager

    this->state = Arch::Install::systemd_setup();
    // ----------------------------------------------------------------
}
/*---------------------------------------------------------------------*/
/*--- systemd_setup - Configuring installation                      ---*/
/*---------------------------------------------------------------------*/
bool Arch::Install::systemd_setup()
{
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"bootctl install\"");

    bool response = false;
    char buf[128];
    std::string result = "";
    std::ofstream myfile;

    std::shared_ptr<FILE> pipe(popen("blkid -s PARTUUID -o value /dev/sda3", "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("--> [ERROR] popen() failed...");
    }
    else
    {
        while (!feof(pipe.get()))
        {
            if (fgets(buf, 128, pipe.get()) != NULL)
            {
                result += buf;
            }
        }

        myfile.open("/mnt/boot/loader/entries/arch.conf");
        if (myfile.is_open())
        {
            myfile << "title Arch Linux\n";
            myfile << "linux /vmlinuz-linux\n";
            myfile << "initrd /initramfs-linux.img\n";

            result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

            myfile << "options root=PARTUUID="+result+" rw quiet splash";
            myfile.close();

            // =====================================================
            // To hide any kernel messages from the console
            myfile.open("/mnt/etc/sysctl.d/20-quiet-printk.conf");
            if (myfile.is_open())
            {
                myfile << "kernel.printk = 3 3 3 3";
                myfile.close();

                // =====================================================
                // Setting keymap
                myfile.open("/mnt/etc/vconsole.conf");
                if(myfile.is_open())
                {
                    myfile << "KEYMAP=" << this->keymap;
                    myfile.close();

                    response = true; // OK, All Done!
                }
                // =====================================================
            }
            // =====================================================
        }
    }

    return(response);
}
/*---------------------------------------------------------------------*/
/*--- GUI_install - Handle GUI installation                         ---*/
/*---------------------------------------------------------------------*/
void Arch::Install::GUI_install()
{
    char *cmdInput = new char;

    do
    {
        std::cout << "\n[Arch Install GUI]" << std::endl;
        std::cout << "\n1 - XFCE" << std::endl;
        std::cout << "0 - EXIT" << std::endl;

        cmdInput = readline("\narch > ");

        if (strcmp(cmdInput, "1") == 0)
        {
            if (Arch::Install::XFCE_install())
            {
                strcpy(cmdInput, "0"); // Exit
            }
            else
            {
                std::cout << "\n--> [ERROR] Failed to install XFCE." << std::endl;
            }
        }

    } while (strcmp(cmdInput, "0") != 0);

    delete[] cmdInput;
}
/*---------------------------------------------------------------------*/
/*--- xfce_install - Install xfce interface                         ---*/
/*---------------------------------------------------------------------*/
bool Arch::Install::XFCE_install()
{
    bool response = false;

    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"pacman -S xorg xorg-xinit pulseaudio pulseaudio-alsa intel-ucode xdg-user-dirs\"");
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"pacman -S "+this->video_drivers+"\"");
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"pacman -S xfce4 xfce4-goodies\"");
    Arch::Install::system_call("arch-chroot /mnt /bin/bash -c \"xdg-user-dirs-update\"");

    // Setting .xinitrc
    std::ofstream of;
    of.open("/mnt/home/"+this->username+"/.xinitrc");
    if (of.is_open())
    {
        of << "#!/bin/sh\n"
        << "#\n"
        << "# ~/.xinitrc\n"
        << "#\n"
        << "# Executed by startx (run your window manager from here)"

        << "\n\nif [ -d /etc/X11/xinit/xinitrc.d ]; then"
        << "\n  for f in /etc/X11/xinit/xinitrc.d/*; do"
        << "\n    [ -x \"$f\" ] && . \"$f\" "
        << "\n  done"
        << "\n  unset f"
        << "\nfi"
        << "\n\nexec startxfce4";

        of.close();

        // Setting .gtkrc-2.0
        of.open("/mnt/home/"+this->username+"/.gtkrc-2.0");
        if (of.is_open())
        {
            of << "style \"xfdesktop-icon-view\" {"
            << "\nXfdesktopIconView::label-alpha = 0"
            << "\nXfdesktopIconView::selected-label-alpha = 170"

            << "\n\nbase[NORMAL] = \"#cccccc\""
            << "\nbase[SELECTED] = \"#cccccc\""
            << "\nbase[ACTIVE] = \"#cccccc\""

            << "\n\nfg[NORMAL] = \"#ffffff\""
            << "\nfg[SELECTED] = \"#000000\""
            << "\nfg[ACTIVE] = \"#000000\""
            << "\n}"
            << "\nwidget_class \"*XfdesktopIconView*\" style \"xfdesktop-icon-view\"";

            response = true;
            of.close();
        }
    }

    return(response);
}
