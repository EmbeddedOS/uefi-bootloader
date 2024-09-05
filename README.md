# uefi-bootloader

A simple UEFI OS loader to load a kernel.

Build the GNU-EFI:

```bash
git submodule update -init
make lib
```

Build applications:

```bash
make app
```

Build final image:

```bash
make image
```

## Emulate with qemu

```bash
make image && ./qemu.sh
```

## UEFI specification

[UEFI spec](https://uefi.org/specs/UEFI/2.10/)
