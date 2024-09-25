#pragma once

#include <efi.h>
#include <efiapi.h>

/* Public defines ------------------------------------------------------------*/
#define MAX_SUPPORTED_LOADER 5

/* Public types --------------------------------------------------------------*/
typedef EFI_STATUS (*kernel_loader)(UINT8 *buffer,
                                    UINT64 size,
                                    void **entry_point);

/* Public function prototypes ------------------------------------------------*/
/**
 * @brief   - uefi_load_kernel - Try to detect kernel file format and load the
 *            kernel into memory.
 *
 * @param   - ImageHandle - EFI Handler.
 * @param   - SystemTable - EFI system table to get protocols.
 * @param   - file        - kernel filename.
 * @param   - entry point - Founded kernel entry point.
 *
 * @return  - EFI_STATUS.
 */
EFI_STATUS uefi_load_kernel(EFI_HANDLE ImageHandle,
                            EFI_SYSTEM_TABLE *SystemTable,
                            const CHAR16 *file,
                            void **entry_point);