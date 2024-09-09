#include <efi.h>
#include <efilib.h>

#include "file.h"

/* Public defines ------------------------------------------------------------*/
#define KERNEL_IMAGE_PATH L"kernel.bin"

/* Public functions ----------------------------------------------------------*/
EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS res = EFI_SUCCESS;
  EFI_FILE_HANDLE fs_volume;
  EFI_FILE_HANDLE file_handle;
  UINT64 file_size = 0;
  UINT8 *buffer = NULL;

  uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2,
                    SystemTable->ConOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  fs_volume = uefi_get_volume(ImageHandle);

  res = uefi_open_file(fs_volume, KERNEL_IMAGE_PATH, &file_handle);
  if (res != EFI_SUCCESS)
  {
    Print(L"Failed to open %s\n", KERNEL_IMAGE_PATH);
    goto exit;
  }

  file_size = uefi_get_file_size(file_handle);
  buffer = AllocatePool(file_size);

  res = uefi_read_file(file_handle, buffer, file_size);
  if (res != EFI_SUCCESS)
  {
    Print(L"Failed to read %s\n", KERNEL_IMAGE_PATH);
    goto exit;
  }

  Print(L"Header: [0x%2X][0x%2X][0x%2X][0x%2X]\n", buffer[0], buffer[1], buffer[2], buffer[3]);

exit:
  while (1)
  {
    /* code */
  }

  return res;
}