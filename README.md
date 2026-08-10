# <p align="center" dir="auto">FurOS64</p>
<div align="center" dir="auto">
  <img src="https://img.shields.io/github/contributors/ivinexo1/FurOS64"></img>
  <img src="https://img.shields.io/github/commit-activity/w/ivinexo1/FurOS64"></img>
  <img src="https://img.shields.io/github/stars/ivinexo1/FurOS64"></img>
</div>

> [!WARNING]
> The deprecated (32 bit) version of this project can be found [here!](https://github.com/ivinexo1/FurOS)

> [!NOTE]
> This is still (very) early in development, we didn't even port everything over yet.

## Setup (installation)

```
sudo pacman -Sy gcc
sudo pacman -Sy cmake
sudo pacman -Sy make
sudo pacman -Sy mtools
sudo pacman -Sy qemu
```

## How to use

```
rm build -rf
cmake -B build
```
```
qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive file=fat.img,if=ide,format=raw
```

# Potential issues

`$'\r'` getting error similar to this can be fixed by changing EOL conversion to LF

~remove `-d int,mmu` z qemu cmd lebo windows ma skill issue~
