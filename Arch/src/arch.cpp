#include "../lib/arch.h"

Arch::Arch() {
    if(checkConnection() != 0) {
        std::cout << "--> [ERROR] No internet connection..." << std::endl;
        std::cout << "--> [ERROR] Installation aborted..." << std::endl;
    } else {
        if(check_partitions() != 0) {
            std::cout << "--> [ERROR] Wrong partitions table..." << std::endl;
            std::cout << "--> The partitions configuration must be: "
                << "sda1(boot)[EF00] 500MB, sda2(swap)[8200] ?G, "
                << "sda3(arch)[8300] ?G" << std::endl;
        } else {
            // ----------------------------------------------------------------
            // ... [OK] Start Installation ...
            system_call("mkfs.vfat /dev/sda1"); // formating boot partition
            system_call("mkswap /dev/sda2"); // making swap on sda2
            system_call("swapon /dev/sda2"); // swapon on sda2
            system_call("mkfs.ext4 /dev/sda3"); // format arch partition
            system_call("timedatectl set-ntp true"); // update clock
            system_call("pacman -Syy"); // synchronizing package
            if(ArchInstall()) {
                // ... [OK] Installation Successfully ...
                std::cout << "--> [OK] Arch Linux successfully installed!" << std::endl;
            }
            // ----------------------------------------------------------------
        }
    }
}

Arch::~Arch() {

}

void Arch::system_call(const std::string arg) {
    int status = system(arg.c_str());
    if(status != 0) {
        std::cout << "--> [ERROR] System call failed..." << std::endl;
        std::cout << "--> [ERROR] Installation aborted..." << std::endl;
        exit(1);
    }
}

bool Arch::checkConnection() {
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    return(getaddrinfo("www.google.com", "80", &host_info, &host_info_list));
}

// Check the partition table to see if it is in the correct software standard
int Arch::check_partitions() {
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

bool Arch::ArchInstall() {
    bool response = true;

    // ----------------------------------------------------------------
    system_call("mount /dev/sda3 /mnt");
    system_call("mkdir /mnt/boot");
    system_call("mount /dev/sda1 /mnt/boot");
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    system_call("pacstrap -i /mnt base base-devel"); // install arch base
    system_call("genfstab -U -p /mnt >> /mnt/etc/fstab"); // generate fstab file
    system_call("arch-chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/"+zone_info+" /etc/localtime\""); // timezone
    system_call("arch-chroot /mnt /bin/bash -c \"hwclock --systohc --utc\""); // generate /etc/adjtime
    system_call("arch-chroot /mnt /bin/bash -c \"echo "+language+" UTF-8 > /etc/locale.gen\""); // locale config
    system_call("arch-chroot /mnt /bin/bash -c \"locale-gen\""); // locale config
    system_call("arch-chroot /mnt /bin/bash -c \"echo LANG="+language+" > /etc/locale.conf\""); // locale config
    system_call("arch-chroot /mnt /bin/bash -c \"export LANG="+language+"\""); // locale config
    system_call("arch-chroot /mnt /bin/bash -c \"echo "+hostname+" > /etc/hostname\""); // hostname
    system_call("arch-chroot /mnt /bin/bash -c \"pacman -S wireless_tools wpa_supplicant wpa_actiond\""); // network config
    system_call("arch-chroot /mnt /bin/bash -c \"systemctl enable dhcpcd\""); // network config
    system_call("arch-chroot /mnt /bin/bash -c \"echo \"root:"+rootPassword+"\" | sudo chpasswd\""); // setting root password

    response = systemd_setup();

    system_call("umount -R /mnt/boot");
    system_call("umount -R /mnt");
    // ----------------------------------------------------------------

    return(response);
}

bool Arch::systemd_setup() {
    system_call("arch-chroot /mnt /bin/bash -c \"bootctl install\"");

    bool response = false;
    char buf[128];
    std::string result = "";
    std::ofstream myfile;

    std::shared_ptr<FILE> pipe(popen("blkid -s PARTUUID -o value /dev/sda3", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("--> [ERROR] popen() failed...");
    } else {
        while (!feof(pipe.get())) {
            if (fgets(buf, 128, pipe.get()) != NULL)
                result += buf;
        }

        myfile.open("/mnt/boot/loader/entries/arch.conf");
        if(myfile.is_open()) {
            myfile << "title Arch Linux\n";
            myfile << "linux /vmlinuz-linux\n";
            myfile << "initrd /initramfs-linux.img\n";

            result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

            myfile << "options root=PARTUUID="+result+" rw quiet splash";
            myfile.close();

            // To hide any kernel messages from the console
            myfile.open("/mnt/etc/sysctl.d/20-quiet-printk.conf");
            if(myfile.is_open()) {

                myfile << "kernel.printk = 3 3 3 3";
                myfile.close();

                response = true;
            }
        }
    }

    return(response);
}
