#pragma once

#include <efi.h>
#include <efiapi.h>

#define MAX_SUPPORTED_LOADER 5

typedef EFI_STATUS (*kernel_loader)(UINT8 *buffer,
                                    UINT64 size,
                                    void **entry_point);

EFI_STATUS uefi_load_kernel(EFI_HANDLE ImageHandle,
                            EFI_SYSTEM_TABLE *SystemTable,
                            const CHAR16 *file,
                            void **entry_point);