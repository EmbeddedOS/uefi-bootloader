#pragma once

#include <efi.h>
#include <efilib.h>

/* Public defines ------------------------------------------------------------*/

/* Public types --------------------------------------------------------------*/
typedef enum
{
    ELF64_E_EI_OSABI_SYSTEM_V = 0x00,
    ELF64_E_EI_OSABI_HP_UX = 0x01,
    ELF64_E_EI_OSABI_NETBSD = 0x02,
    ELF64_E_EI_OSABI_LINUX = 0x03,
    ELF64_E_EI_OSABI_GNU_HURD = 0x04,
    ELF64_E_EI_OSABI_SOLARIS = 0x06,
    ELF64_E_EI_OSABI_AIX = 0x07,
    ELF64_E_EI_OSABI_IRIX = 0x08,
    ELF64_E_EI_OSABI_FREEBSD = 0x09,
    ELF64_E_EI_OSABI_TRU64 = 0x0A,
    ELF64_E_EI_OSABI_NOVELL_MODESTO = 0x0B,
    ELF64_E_EI_OSABI_OPENBSD = 0xC,
    ELF64_E_EI_OSABI_OPENVMS = 0x0D,
    ELF64_E_EI_OSABI_NONSTOP_KERNEL = 0x0E,
    ELF64_E_EI_OSABI_AROS = 0x0F,
    ELF64_E_EI_OSABI_FENIXOS = 0x10,
    ELF64_E_EI_OSABI_NUXI_CLOUDABI = 0x11,
    ELF64_E_EI_OSABI_STRATUS_TECHNOLOGIES_OPENVOS = 0x12
} elf64_e_os_abi_e;

typedef enum
{
    ELF64_E_TYPE_ET_NONE = 0x00,
    ELF64_E_TYPE_ET_REL = 0x01,
    ELF64_E_TYPE_ET_EXEC = 0x02,
    ELF64_E_TYPE_ET_DYN = 0x03,
    ELF64_E_TYPE_ET_CORE = 0x04,
    ELF64_E_TYPE_ET_LOOS = 0xFE00,
    ELF64_E_TYPE_ET_HIOS = 0xFEFF,
    ELF64_E_TYPE_ET_LOPROC = 0xFF00,
    ELF64_E_TYPE_ET_HIPROC = 0xFFFF
} elf64_e_type_e;

typedef enum
{
    ELF64_P_PF_X = 0x01,
    ELF64_P_PF_W = 0x02,
    ELF64_P_PF_R = 0x03
} elf64_p_flags_e;

typedef enum
{
    ELF64_P_PT_NULL = 0x00000000,
    ELF64_P_PT_LOAD = 0x00000001,
    ELF64_P_PT_DYNAMIC = 0x00000002,
    ELF64_P_PT_INTERP = 0x00000003,
    ELF64_P_PT_NOTE = 0x00000004,
    ELF64_P_PT_SHLIB = 0x00000005,
    ELF64_P_PT_PHDR = 0x00000006,
    ELF64_P_PT_TLS = 0x00000007,
    ELF64_P_PT_LOOS = 0x60000000,
    ELF64_P_PT_HIOS = 0x6FFFFFFF,
    ELF64_P_PT_LOPROC = 0x70000000,
    ELF64_P_PT_HIPROC = 0x7FFFFFFF,
} elf64_p_type_e;

typedef struct
{
    struct e_ident_t
    {
        UINT8 ei_magic0;      /* Magic number 0: '0x7F'.                    */
        UINT8 ei_magic1;      /* Magic number 1: 'E'.                       */
        UINT8 ei_magic2;      /* Magic number 2: 'L'.                       */
        UINT8 ei_magic3;      /* Magic number 3: 'F'.                       */
        UINT8 ei_class;       /* Signify 32- or 64-bit format.              */
        UINT8 ei_data;        /* Signify little or big endianness.          */
        UINT8 ei_version;     /* Signify Original and current version.      */
        UINT8 ei_os_abi;      /* Identifies target Operating System.        */
        UINT8 ei_abi_version; /* Specify the ABI version.                   */
        UINT8 ei_pad[7];      /* Reversed padding bytes.                    */
    } e_ident;                /* Identifies field.                          */

    UINT16 e_type;      /* Identifies object type.                            */
    UINT16 e_machine;   /* Specifies target instruction set arch.             */
    UINT32 e_version;   /* Signify original version.                          */
    UINT64 e_entry;     /* Memory address of the entry point.                 */
    UINT64 e_phoff;     /* Points to the start of the program header table.   */
    UINT64 e_shoff;     /* Points to the start of the section header table.   */
    UINT32 e_flags;     /* Flags, depends on architecture.                    */
    UINT16 e_ehsize;    /* Size of this header.                               */
    UINT16 e_phentsize; /* Size of a program header table entry.              */
    UINT16 e_phnum;     /* Number of entries in the program header table.     */
    UINT16 e_shentsize; /* Size of a section header table entry.              */
    UINT16 e_shnum;     /* The number of entries in the section header table. */
    UINT16 e_shstrndx;  /* Index of the section header table entry.           */
} __attribute__((packed)) elf64_header_t;

typedef struct
{
    UINT32 p_type;    /* Identifies the type of the segment.              */
    UINT32 p_flags64; /* Segment-dependent flags.                         */
    UINT64 p_offset;  /* Offset of the segment in memory.                 */
    UINT64 p_vaddr;   /* Virtual address of the segment in memory.        */
    UINT64 p_paddr;   /* On systems where physical is relevant.           */
    UINT64 p_filesz;  /* Size in bytes of the segment in the file image.  */
    UINT64 p_memsz;   /* Size in bytes of the segment in memory.          */
    UINT64 p_align;   /* Signify no alignment.                            */
} __attribute__((packed)) elf64_program_header_t;

typedef struct
{
    UINT32 sh_name;
    UINT32 sh_type;
    UINT64 sh_flags;
    UINT64 sh_addr;
    UINT64 sh_offset;
    UINT64 sh_size;
    UINT32 sh_link;
    UINT32 sh_info;
    UINT64 sh_addralign;
    UINT64 sh_entsize;
} __attribute__((packed)) elf64_section_header_t;

/* Public function prototypes ------------------------------------------------*/
EFI_STATUS load_elf_kernel(UINT8 *buffer,
                           UINT64 size,
                           void **entry_point);