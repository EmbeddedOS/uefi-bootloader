#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS res = EFI_SUCCESS;

  uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2, SystemTable->ConOut, EFI_TEXT_ATTR(EFI_YELLOW, EFI_GREEN));

  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  uefi_call_wrapper(SystemTable->ConOut->OutputString, 2, SystemTable->ConOut, L"Hello, world!\r\n\r\n");

  while (1)
  {
    /* code */
  }
  
  return EFI_SUCCESS;
}