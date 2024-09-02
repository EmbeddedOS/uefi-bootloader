build-gnu-efi:
	make -C gnu-efi
kernel:
	gcc -ffreestanding -c kernel.c -o kernel.o -fno-pie
	ld -o kernel.bin kernel.o -nostdlib --oformat=binary -Ttext=0x10000
clean:
	rm -rf *.o *.so *img *.efi