# uefi-bootloader

A simple UEFI OS loader to load a kernel.

Build the GNU-EFI:

```bash
git submodule update -init
make build-gnu-efi
```

Build the application:

```bash
# Create an OS image with EFI system partition and FAT32.
./create-img.sh

# Build kernel, UEFI OS loader and put into the image.
./build.sh

# Emulator system with QEMU.
./qemu.sh
```

## UEFI specification

[UEFI spec](https://uefi.org/specs/UEFI/2.10/)
