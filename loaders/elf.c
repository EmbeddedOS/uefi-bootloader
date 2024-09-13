#include "elf.h"

void print_elf_info(UINT8 *buffer);

void print_elf_info(UINT8 *buffer)
{
    elf64_header_t *header = (elf64_header_t *)buffer;
    elf64_program_header_t *program_header =
        (elf64_program_header_t *)(buffer + header->e_phoff);

    Print(L"ELF header type: %d\n", header->e_type);
    Print(L"ELF header machine: %d\n", header->e_machine);
    Print(L"ELF header entry: 0x%lx\n", header->e_entry);

    /* Print all segment info. */
    for (INT32 i = 0; i < header->e_phnum; i++, program_header++)
    {
        Print(L"Segment %d type: 0x%x, flag: %d, "
              "offset: 0x%lx, aligned: 0x%lx\n",
              i,
              program_header->p_type,
              program_header->p_flags64,
              program_header->p_offset,
              program_header->p_align);
    }
}

EFI_STATUS load_elf_kernel(UINT8 *buffer,
                           UINT64 size,
                           void **entry_point)
{
    elf64_header_t *header = (elf64_header_t *)buffer;
    elf64_program_header_t *program_header = NULL;

    if (header->e_ident.ei_magic0 != 0x7F ||
        header->e_ident.ei_magic1 != 'E' ||
        header->e_ident.ei_magic2 != 'L' ||
        header->e_ident.ei_magic3 != 'F')
    {
        Print(L"kernel is not in EFL format.\n");
        return EFI_LOAD_ERROR;
    }

    if (header->e_type != ELF64_E_TYPE_ET_DYN)
    { // Only access shared object.
        Print(L"ELF type is not supported: %d\n", header->e_type);
        return EFI_LOAD_ERROR;
    }

    Print(L"Loading ELF kernel...\n");
    print_elf_info(buffer);

    program_header = (elf64_program_header_t *)(buffer + header->e_phoff);

    return EFI_SUCCESS;
}
