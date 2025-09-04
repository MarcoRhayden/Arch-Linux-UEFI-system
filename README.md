# Arch Linux Installer — v2.2.0
![alt text](http://ninjadolinux.com.br/wp-content/uploads/2016/09/arch-linux.jpg)

> A modular, data-driven Arch Linux installer using **CMake**, **YAML manifests**, and a simple **pipeline/runner** architecture.  
> Supports XFCE and i3 desktops out of the box. Includes a **dry-run** mode for safe validation.

---

## UEFI Partitioning (required)

Create the following GPT partitions (you can use `gdisk`):

- `/dev/sda1` — **EFI System** (type `EF00`), ~500MB  
- `/dev/sda2` — **swap** (type `8200`), size = your choice  
- `/dev/sda3` — **root** (type `8300`), `ext4`, remaining space

> ⚠️ This installer will format and mount these partitions. Double-check device names.

---

## Getting the Source (from Arch Live ISO or any Linux)

```bash
git clone https://github.com/MarcoRhayden/Arch-Linux-UEFI-system.git
cd Arch-Linux-UEFI-system/arch-installer
```

---

## Prerequisites

On Arch (live environment):

```bash
pacman -Syy
pacman -S --needed gcc cmake make git base-devel yaml-cpp
```

---

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

The binary will be at `build/arch-installer`.

> If your system installs `yaml-cpp` in a nonstandard prefix, pass it to CMake:
> ```bash
> cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/yaml-cpp
> ```

---

## Run

### Dry-run (prints what would be executed; **no changes**)

```bash
./build/arch-installer   --desktop=i3   --dev-boot=/dev/sda1   --dev-swap=/dev/sda2   --dev-root=/dev/sda3   --dry-run=1
```

### Actual install

```bash
./build/arch-installer   --desktop=xfce   --dev-boot=/dev/sda1   --dev-swap=/dev/sda2   --dev-root=/dev/sda3
```

Supported desktops: `xfce`, `i3`.

---

## How It Works

- **Context** holds variables (user, locale, keymap, etc.).
- **Pipeline** executes a sequence of **Steps** (`run`, `chroot`, `file`, `service`, `userset`, `capture`). 
- **Runner** executes shell commands (real or **dry-run**).
- **Manifests (YAML)** describe steps declaratively:
  - `src/manifests/base.yml`
  - `src/manifests/bootloader.yml`
  - `src/manifests/desktops/xfce.yml`
  - `src/manifests/desktops/i3.yml`

At build time, manifests are copied next to the binary, so the program can load them with `manifests/...`.

---

## Configuration (Variables)

Manifests interpolate variables like `${user}`, `${lang}`, `${zone}` from the **Context**.  
Defaults live in `include/core/Context.hpp`. Typical variables:

- `hostname` (e.g., `arch`)
- `user`, `user_password`, `root_password`
- `lang` (e.g., `en_US.UTF-8` or `pt_BR.UTF-8`)
- `zone` (e.g., `America/Sao_Paulo`)
- `keymap` (e.g., `br-abnt2`)
- `video_drivers` (e.g., `xf86-video-intel mesa`)

Devices are also exposed as:
- `${dev_boot}`, `${dev_swap}`, `${dev_root}`

You can change defaults in code or add a simple CLI/config loader later.

---

## Repository Layout

```
arch-installer/
├─ include/
│  ├─ Arch.hpp
│  └─ core/
│     ├─ Command.hpp
│     ├─ Context.hpp
│     ├─ Manifest.hpp
│     ├─ NetCheck.hpp
│     ├─ PartitionCheck.hpp
│     ├─ Pipeline.hpp
│     └─ Runner.hpp
├─ src/
│  ├─ Arch.cpp
│  ├─ main.cpp
│  ├─ core/
│  │  ├─ Manifest.cpp
│  │  ├─ NetCheck.cpp
│  │  ├─ PartitionCheck.cpp
│  │  ├─ Pipeline.cpp
│  │  └─ Runner.cpp
│  └─ manifests/
│     ├─ base.yml
│     ├─ bootloader.yml
│     └─ desktops/
│        ├─ xfce.yml
│        └─ i3.yml
└─ CMakeLists.txt
```

---

## Build System Notes (CMake)

- Ensure `yaml-cpp` is discoverable. If needed, tell CMake where to find it:
  ```bash
  cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/yaml-cpp
  ```
- If you prefer, use `FetchContent` to embed `yaml-cpp` at configure time.
- Enable compile commands for better IDE integration:
  ```cmake
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  ```

The project copies `src/manifests/` into the build directory so the executable can load YAMLs with relative paths (`manifests/...`).

---

## Safety Notes

- **This installer will format and mount** the devices you pass; use at your own risk.
- Always test first with `--dry-run=1`.
- Prefer `PARTUUID`-based mounting for robustness (the bootloader manifest captures it).

---

## Adding a New Desktop

Create a new manifest at `src/manifests/desktops/<your-desktop>.yml`, then run with:
```bash
./build/arch-installer --desktop=<your-desktop> --dev-boot=... --dev-swap=... --dev-root=...
```
Use the existing `xfce.yml` / `i3.yml` as templates.

---
