cd ..
cmake --build build
cd run
dd if=/dev/zero of=fat.img bs=1k count=1440
mformat -i fat.img -f 1440 ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img ../build/kernel ::/
mcopy -i fat.img ../build/efi/EFI_STUB.efi ::/
mcopy -i fat.img ../build/efi/BOOTX64.efi ::/EFI/BOOT
# qemu-system-x86_64 -d int,mmu -no-reboot -no-shutdown -cpu qemu64 -bios OVMF.fd -drive file=fat.img,if=ide,format=raw
# aka je pointa printovania ? + win11 sa posere z toho
# debugging and logging of triple faults
qemu-system-x86_64 -d int -cpu qemu64 -bios OVMF.fd -drive file=fat.img,if=ide,format=raw
