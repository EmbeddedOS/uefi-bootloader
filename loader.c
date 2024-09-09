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


exit:
  while (1)
  {
    /* code */
  }

  return res;
}