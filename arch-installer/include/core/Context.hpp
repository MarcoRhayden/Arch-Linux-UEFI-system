#pragma once
#include <string>
#include <unordered_map>

struct Context {
  std::unordered_map<std::string, std::string> vars = {
    {"hostname", "arch"},
    {"user", "rhayden"},
    {"user_password", "USERPASSWORD"},
    {"root_password", "ROOTPASSWORD"},
    {"lang", "en_US.UTF-8"},
    {"zone", "America/Sao_Paulo"},
    {"keymap", "br-abnt2"},
    {"video_drivers", "xf86-video-nouveau mesa xf86-video-intel"},
    {"user_packages",
     "wireless_tools wpa_supplicant wpa_actiond bluez bluez-utils pulseaudio-bluetooth blueman "
     "firefox ttf-dejavu p7zip zip unzip tumbler libmpeg2 ffmpeg ffmpegthumbnailer gvfs gvfs-smb networkmanager "
     "network-manager-applet dhclient xfce4-notifyd gnome-keyring "
     "pavucontrol xarchiver git sqlitebrowser vlc qt4 xpdf galculator ntfs-3g boost openssl"}
  };

  std::string devBoot = "/dev/sda1";
  std::string devSwap = "/dev/sda2";
  std::string devRoot = "/dev/sda3";

  std::string mountRoot = "/mnt";

  std::string desktop = "i3";

  bool dryRun =
  #ifdef ARCHI_DRY_RUN
    true;
  #else
    false;
  #endif
};
