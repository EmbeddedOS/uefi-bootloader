#include <efi.h>
#include <efilib.h>

#include "kernel.h"
#include "loaders/loader.h"

/* Public defines ------------------------------------------------------------*/
#define KERNEL_IMAGE_PATH L"kernel.elf"

/* Public functions prototypes -----------------------------------------------*/
EFI_GRAPHICS_OUTPUT_PROTOCOL *
uefi_get_graphic_output_protocol();

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
  boot_params_t kernel_params = {0};
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
  void *entry_point = NULL;

  InitializeLib(ImageHandle, SystemTable);

  /* 1. Configure screen colours. */
  uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2,
                    SystemTable->ConOut,
                    EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  /* 2. Load kernel. */
  res = uefi_load_kernel(ImageHandle, SystemTable,
                         KERNEL_IMAGE_PATH, &entry_point);

  /* 3. Make kernel parameters. */
  gop = uefi_get_graphic_output_protocol();
  if (gop == NULL)
  {
    goto uefi_get_graphic_output_protocol_failure;
  }

  kernel_params.graphic_out_protocol = *gop->Mode;


  /* 4. Jump to kernel. */
  ((kernel_entry)entry_point)(&kernel_params);

uefi_get_graphic_output_protocol_failure:
exit:
  while (1)
  {
    /* code */
  }

  return res;
}

EFI_GRAPHICS_OUTPUT_PROTOCOL *
uefi_get_graphic_output_protocol()
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_GUID guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

  status = uefi_call_wrapper(BS->LocateProtocol, 3, &guid, NULL, (void **)&gop);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to locate graphics output protocol: %d\n", status);
    return NULL;
  }

  return gop;
}
