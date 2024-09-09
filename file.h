#include <efi.h>
#include <efilib.h>

EFI_FILE_HANDLE uefi_get_volume(EFI_HANDLE image);

UINT64 uefi_get_file_size(EFI_FILE_HANDLE file_handle);

EFI_STATUS uefi_open_file(EFI_FILE_HANDLE volume,
                          CHAR16 *filename,
                          EFI_FILE_HANDLE *file_handle);

EFI_STATUS uefi_close_file(EFI_FILE_HANDLE file_handle);

EFI_STATUS uefi_read_file(EFI_FILE_HANDLE file_handle, UINT8 *buffer, UINT64 size);

EFI_STATUS uefi_load_kernel_image(EFI_FILE_HANDLE volume,
                                  CHAR16 *filename,
                                  EFI_PHYSICAL_ADDRESS *kernel_entry_point);
