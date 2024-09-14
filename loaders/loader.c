#include "binary.h"
#include "elf.h"
#include "../file.h"

#include "loader.h"

 struct kernel_loader_t
{
    kernel_loader loader;
    CHAR16 *name;
}  loaders[MAX_SUPPORTED_LOADER] = {
    {.name = L"ELF Loader", .loader = load_elf_kernel},

    /* Try to load by binary loader finally.*/
    {.name = L"Flat Binary Loader", .loader = load_binary_kernel},
    {.name = NULL, .loader = NULL}};

EFI_STATUS uefi_load_kernel(EFI_HANDLE ImageHandle,
                            EFI_SYSTEM_TABLE *SystemTable,
                            const CHAR16 *file,
                            void **entry_point)
{
    EFI_STATUS res = EFI_SUCCESS;
    EFI_FILE_HANDLE fs_volume;
    EFI_FILE_HANDLE file_handle;
    UINT64 file_size = 0;
    UINT8 *buffer = NULL;

    fs_volume = uefi_get_volume(ImageHandle);

    res = uefi_open_file(fs_volume, file, &file_handle);
    if (res != EFI_SUCCESS)
    {
        Print(L"Failed to open %s\n", file);
        goto exit;
    }

    file_size = uefi_get_file_size(file_handle);
    buffer = AllocatePool(file_size);

    res = uefi_read_file(file_handle, buffer, file_size);
    if (res != EFI_SUCCESS)
    {
        Print(L"Failed to read %s\n", file);
        goto uefi_read_file_failure;
    }

    for (int i = 0; i < MAX_SUPPORTED_LOADER; i++)
    {
        if (loaders[i].loader == NULL)
        {
            Print(L"No loader can load the kernel\n");
            break;
        }

        if (loaders[i].loader(buffer, file_size, entry_point) == EFI_SUCCESS)
        {
            Print(L"%s loaded the kernel, entry point: 0x%lx\n",
            loaders[i].name,
            *(UINT64 *)entry_point);
            break;
        }
    }

uefi_read_file_failure:
    uefi_close_file(file_handle);
    FreePool(buffer);
exit:
    return res;
}