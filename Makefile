CC=gcc
LD=ld
INC=-Ignu-efi/inc
CFLAGS=-fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args
LDFLAGS=-shared -Bsymbolic
LDLIB_DIRS=-Lgnu-efi/x86_64/lib -Lgnu-efi/x86_64/gnuefi
LDLIBS=-lgnuefi -lefi
LD_LINKER_FILE=gnu-efi/gnuefi/elf_x86_64_efi.lds
LD_STARTUP_FILE=gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o
OBJCOPY_FLAGS=-j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10
BOOTLOADER_IMG=loader.efi
OBJS= loader.o file.o
OS_IMAGE=uefi.img

lib:
	sudo make -C gnu-efi install

app:$(BOOTLOADER_IMG)
	echo "Built BOOTLOADER_OBJ"

kernel:
	gcc -ffreestanding -c kernel.c -o kernel.o -fno-pie
	ld -o kernel.bin kernel.o -nostdlib --oformat=binary -Ttext=0x10000

image: $(OS_IMAGE) $(BOOTLOADER_IMG)
	sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop99 uefi.img
	sudo mount /dev/loop99 /mnt
	sudo mkdir -p /mnt/EFI/BOOT/
	sudo cp $(BOOTLOADER_IMG) /mnt/EFI/BOOT/BOOTX64.EFI
	sudo umount /mnt
	sudo losetup -d /dev/loop99

all: lib image kernel

clean:
	rm -rf *.o *.so *img *.efi

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

%.so: $(OBJS)
	$(LD) $(LDFLAGS) $(LDLIB_DIRS) -T$(LD_LINKER_FILE) $(LD_STARTUP_FILE) $(OBJS) -o $@ $(LDLIBS)

%.efi: %.so
	objcopy $(OBJCOPY_FLAGS) $< $@

%.img:
	sudo ./create-img.sh