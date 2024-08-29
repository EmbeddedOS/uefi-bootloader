# 1. Compile the application.
gcc -Ignu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c loader.c -o loader.o

# 2. Link with UEFI libraries.
ld -shared -Bsymbolic -Lgnu-efi/x86_64/lib -Lgnu-efi/x86_64/gnuefi -Tgnu-efi/gnuefi/elf_x86_64_efi.lds gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o loader.o -o loader.so -lgnuefi -lefi

# 3. Converting Shared Object to EFI executable.
objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 loader.so loader.efi

# 4. Create an image and copy binaries.
bash ./create-img.sh
sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop99 uefi.img
sudo mount /dev/loop99 /mnt

sudo mkdir -p /mnt/EFI/BOOT/
sudo cp loader.efi /mnt/EFI/BOOT/BOOTX64.EFI

sudo umount /mnt
sudo losetup -d /dev/loop99
