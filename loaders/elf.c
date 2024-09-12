#include "elf.h"

EFI_STATUS load_elf_kernel(UINT8 *buffer,
                           UINT64 size,
                           void **entry_point)
{
    elf64_header_t *header = buffer;

    if (strncmpa(&buffer[1], "ELF", 3) != 0)
    {
        Print(L"kernel is not in EFL format.\n");
        return EFI_LOAD_ERROR;
    }

    Print(L"Loading ELF kernel...\n");

    return EFI_SUCCESS;
}