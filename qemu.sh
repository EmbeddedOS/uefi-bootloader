bash ./build.sh
qemu-system-x86_64 -cpu qemu64 -bios /usr/share/qemu/OVMF.fd -drive file=uefi.img,if=ide
