#include "binary.h"

EFI_STATUS load_binary_kernel(UINT8 *buffer,
                              UINT64 size,
                              void **entry_point)
{
    UINT8 * kernel_buffer = NULL;
    kernel_buffer = AllocatePool(size);
    uefi_call_wrapper(CopyMem, 3, kernel_buffer, buffer, size);

    *entry_point = (VOID *)kernel_buffer;
    return EFI_SUCCESS;
}