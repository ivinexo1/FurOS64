cd ..
cmake --build build
cd run
dd if=/dev/zero of=fat.img bs=1k count=1440
mformat -i fat.img -f 1440 ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img ./hello.str ::/
mcopy -i fat.img ../build/BOOTX64.efi ::/EFI/BOOT
qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive file=fat.img,if=ide,format=raw
