#pragma once

#include <efi.h>
#include <efilib.h>

EFI_STATUS load_binary_kernel(UINT8 *buffer,
                              UINT64 size,
                              void **entry_point);