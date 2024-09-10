#include <efi.h>
#include <efilib.h>

#include "kernel.h"
#include "file.h"

/* Public defines ------------------------------------------------------------*/
#define KERNEL_IMAGE_PATH L"kernel.bin"

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
  kernel_entry kernel_entry_point = NULL;

  InitializeLib(ImageHandle, SystemTable);

  /* 1. Configure screen colours. */
  uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2,
                    SystemTable->ConOut,
                    EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  /* 2. Load the kernel file. */
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

  /* 3. Parse the kernel format. */
  Print(L"Header: [0x%2x][0x%2x][0x%2x][0x%2x]\n",
      buffer[0], buffer[1], buffer[2], buffer[3]);

  if (strncmpa(&buffer[1], "ELF", 3) == 0)
  { // ELF format start with 0x7F followed by ELF(45 4c 46) in ASCII.
    Print(L"%s is in EFL format.\n", KERNEL_IMAGE_PATH);

  } else 
  { // Flat binary format.
    Print(L"%s is in flat binary format.\n", KERNEL_IMAGE_PATH);
    kernel_entry_point = (kernel_entry)buffer;
  }

  /* 4. Make kernel parameters. */
  gop = uefi_get_graphic_output_protocol();
  if (gop == NULL)
  {
    goto uefi_get_graphic_output_protocol_failure;
  }

  kernel_params.graphic_out_protocol = *gop->Mode;

  Print(L"Jump to kernel\n");
  kernel_entry_point(&kernel_params);

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
