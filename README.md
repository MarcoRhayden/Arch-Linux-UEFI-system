# Arch Linux Installation V2.2.0
![alt text](http://ninjadolinux.com.br/wp-content/uploads/2016/09/arch-linux.jpg)
--> Install Arch Linux has never been easier!

## Partitioning Guide:   [UEFI system]

First of all, you need to partitioning your disk with program requeriments, you can use **gdisk** to do that.

• **[sda1]** Create a **boot** partition flag code EF00

• **[sda2]** Create a **swap** partition flag code 8200

• **[sda3]** Create a **arch** partition ext4 code 8300


### Installation:

**1)** Download the repository to your Arch live

```
wget https://github.com/MarcoRhayden/Arch Linux UEFI system.git
```

**2)** Synchronize the packages

```
pacman -Syy
```

**3)** Install **gcc compiler**

```
pacman -S gcc
```

**4)** Open the **arch.h** header and setup your own config **(IMPORTANT)**


**5**) Make sure your **partition table** is the same as the previous **partitioning guide**


**6)** Go to folder Arch compile the project and start installation

```
gcc -x c++ -lstdc++ core.cpp src/*.cpp -oArch.out
```
```
./Arch.out
```
