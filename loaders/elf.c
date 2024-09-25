#include "elf.h"

/* Private defines -----------------------------------------------------------*/
#define PAGE_SIZE 4096

/* Private function prototypes -----------------------------------------------*/
void print_elf_info(UINT8 *buffer);

/* Private functions ---------------------------------------------------------*/
void print_elf_info(UINT8 *buffer)
{
    elf64_header_t *header = (elf64_header_t *)buffer;
    elf64_program_header_t *program_header =
        (elf64_program_header_t *)(buffer + header->e_phoff);

    Print(L"ELF type: %d, machine: %d, entry: %lx\n",
          header->e_type,
          header->e_machine,
          header->e_entry);

    /* Print all segment info. */
    for (INT32 i = 0; i < header->e_phnum; i++, program_header++)
    {
        Print(L"ELF section:%d type: 0x%x, flag: 0x%x, "
              "offset: 0x%lx, vaddr: 0x%x\n",
              i,
              program_header->p_type,
              program_header->p_flags64,
              program_header->p_offset,
              program_header->p_vaddr,
              program_header->p_filesz);
    }
}

/* Public functions ----------------------------------------------------------*/
EFI_STATUS load_elf_kernel(UINT8 *buffer,
                           UINT64 size,
                           void **entry_point)
{
    EFI_STATUS res = EFI_SUCCESS;
    elf64_header_t *header = (elf64_header_t *)buffer;
    elf64_program_header_t *program_header = NULL;
    UINT64 max_alignment = PAGE_SIZE;
    UINT64 mem_start = UINT64_MAX;
    UINT64 mem_end = 0;
    uint32_t needed_memory_size = 0;
    VOID *program_memory_buffer = NULL;

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

    /* 1. Calculate memory bounds for all program sections. */
    program_header = (elf64_program_header_t *)(buffer + header->e_phoff);
    for (INT32 i = 0; i < header->e_phnum; i++, program_header++)
    {
        if (program_header->p_type == ELF64_P_PT_LOAD)
        { // Handle loadable segment only.
            if (program_header->p_align > max_alignment)
            {
                max_alignment = program_header->p_align;
            }

            UINT64 segment_mem_begin = program_header->p_vaddr;
            UINT64 segment_mem_end =
                program_header->p_vaddr +
                program_header->p_memsz + max_alignment - 1;

            segment_mem_begin &= ~(max_alignment - 1);
            segment_mem_end &= ~(max_alignment - 1);

            if (segment_mem_begin < mem_start)
            {
                mem_start = segment_mem_begin;
            }

            if (segment_mem_end > mem_end)
            {
                mem_end = segment_mem_end;
            }
        }
    }

    needed_memory_size = mem_end - mem_start;

    Print(L"Needed kernel's memory: 0x%x\n", needed_memory_size);

    /* 2. Allocate buffer for program headers. */
    res = uefi_call_wrapper(BS->AllocatePool, 3,
                            EfiLoaderData,
                            needed_memory_size,
                            &program_memory_buffer);
    if (EFI_ERROR(res))
    {
        Print(L"Cannot allocate memory for program sections!\n");
        return res;
    }

    /* 3. Load loadable section into memory. */
    program_header = (elf64_program_header_t *)(buffer + header->e_phoff);
    for (INT32 i = 0; i < header->e_phnum; i++, program_header++)
    {
        if (program_header->p_type == ELF64_P_PT_LOAD)
        { // Handle loadable segment only.

            UINT64 relative_offset = program_header->p_vaddr - mem_start;
            UINT8 *dst = (UINT8 *)program_memory_buffer + relative_offset;
            UINT8 *src = (UINT8 *)buffer + program_header->p_offset;
            UINT32 len = program_header->p_filesz;

            uefi_call_wrapper(CopyMem, 3, dst, src, len);

            Print(L"Loaded %p to %p len: %x, offset: %ld\n",
                  src, dst, len, relative_offset);
        }
    }

    Print(L"Program memory: %p, entry offset: %x, start: 0x%x\n",
          program_memory_buffer, header->e_entry, mem_start);

    /* 4. Update entry point. */
    *entry_point = (VOID *)((UINT8 *)program_memory_buffer +
                            (header->e_entry - mem_start));

    return EFI_SUCCESS;
}
