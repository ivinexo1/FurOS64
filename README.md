# <p align="center" dir="auto">FurOS64</p>
<div align="center" dir="auto">
  <img src="https://img.shields.io/github/contributors/ivinexo1/FurOS64"></img>
  <img src="https://img.shields.io/github/commit-activity/w/ivinexo1/FurOS64"></img>
  <img src="https://img.shields.io/github/stars/ivinexo1/FurOS64"></img>
</div>

32bit version can be found [here](https://github.com/ivinexo1/FurOS), idk what to write here yet we need to port over the stuff first ig

stuff to install [requires linux / WSL]
```
sudo pacman -Sy gcc
sudo pacman -Sy cmake
sudo pacman -Sy make
sudo pacman -Sy mtools
```
also qemu but that can be done in windows

```
rm build -rf
cmake -B build
```

`$'\r'` getting error similar to this can be fixed by doing notepad++ > edit > EOL conversion > LF

remove `-d int,mmu` z qemu cmd lebo windows ma skill issue