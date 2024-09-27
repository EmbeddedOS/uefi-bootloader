#include <efi.h>
#include <efilib.h>

#include "kernel.h"
#include "loaders/loader.h"
#include "protocol.h"

/* Public defines ------------------------------------------------------------*/
#define KERNEL_IMAGE_PATH L"kernel.elf"
#define CUSTOM_PROTOCOL_DATA 123

/* Public functions prototypes -----------------------------------------------*/
EFI_GRAPHICS_OUTPUT_PROTOCOL *
uefi_get_graphic_output_protocol();

EFI_STATUS
uefi_install_custom_protocol(EFI_HANDLE ImageHandle);

CUSTOM_PROTOCOL *
uefi_get_custom_protocol();

EFI_STATUS
uefi_get_mm(memory_map_t *mm);

/**
 * @brief   - Get a character from keyboards.
 */
EFI_INPUT_KEY uefi_get_key(void);

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
  CUSTOM_PROTOCOL *custom_protocol = NULL;
  void *entry_point = NULL;

  InitializeLib(ImageHandle, SystemTable);

  res = uefi_install_custom_protocol(ImageHandle);
  if (EFI_ERROR(res))
  {
    Print(L"Failed to install custom protocol: %d\n", res);
    goto exit;
  }

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

  custom_protocol = uefi_get_custom_protocol();
  if (custom_protocol)
  {
    kernel_params.custom_protocol_data = custom_protocol->data;
  }

  kernel_params.runtime_services = SystemTable->RuntimeServices;
  kernel_params.graphic_out_protocol = *gop->Mode;
  /* 4. Jump to kernel. */
  Print(L"Press any key to enter to kernel...\n");
  uefi_get_key();
  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  res = uefi_get_mm(&kernel_params.mm);
  if (EFI_ERROR(res))
  {
    Print(L"Failed to get memory map: %d\n", res);
    goto uefi_get_mm_failure;
  }

  /* NOTE: Don't do anything between get memory and exit boot services step. */

  res = uefi_call_wrapper(BS->ExitBootServices, 2,
                          ImageHandle,
                          kernel_params.mm.map_key);
  if (EFI_ERROR(res))
  {
    Print(L"Failed to exit boot services: %d\n", res);
    goto ExitBootServices_failure;
  }

  ((kernel_entry)entry_point)(&kernel_params);

ExitBootServices_failure:
uefi_get_mm_failure:
uefi_get_graphic_output_protocol_failure:
  /* TODO: cleanup memory pool. */

exit:
  Print(L"Failed to load kernel, press any key to exit...\n");
  uefi_get_key();

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

EFI_INPUT_KEY uefi_get_key(void)
{
  EFI_EVENT events[1];
  EFI_INPUT_KEY key;
  UINTN index = 0;

  key.ScanCode = 0;
  key.UnicodeChar = u'\0';
  events[0] = ST->ConIn->WaitForKey;

  uefi_call_wrapper(BS->WaitForEvent, 3, 1, events, &index);

  if (index == 0)
  {
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
  }

  return key;
}

EFI_STATUS
uefi_get_mm(memory_map_t *mm)
{
  EFI_STATUS res = EFI_SUCCESS;

  /* 1. Get memory size by pass size 0. */
  res = uefi_call_wrapper(BS->GetMemoryMap, 5,
                          &mm->mm_size,
                          mm->mm_descriptor,
                          &mm->map_key,
                          &mm->descriptor_size,
                          &mm->descriptor_version);
  if (EFI_ERROR(res) && res != EFI_BUFFER_TOO_SMALL)
  {
    return res;
  }

  /* 2. Update new memory size to get. */
  mm->mm_size += mm->descriptor_size * 2;
  res = uefi_call_wrapper(BS->AllocatePool, 3,
                          EfiLoaderData,
                          mm->mm_size,
                          &mm->mm_descriptor);
  if (EFI_ERROR(res))
  {
    return res;
  }

  /* 3. Get memory map. */
  res = uefi_call_wrapper(BS->GetMemoryMap, 5,
                          &mm->mm_size,
                          mm->mm_descriptor,
                          &mm->map_key,
                          &mm->descriptor_version,
                          &mm->descriptor_size);
  if (EFI_ERROR(res))
  {
    uefi_call_wrapper(BS->FreePool, 1, mm->mm_descriptor);
  }

  return res;
}

EFI_STATUS
uefi_install_custom_protocol(EFI_HANDLE ImageHandle)
{
  EFI_STATUS res = EFI_SUCCESS;
  CUSTOM_PROTOCOL *custom_protocol = NULL;
  EFI_GUID guid = EFI_CUSTOM_PROTOCOL_GUID;

  res = uefi_call_wrapper(BS->AllocatePool, 3,
                          EfiBootServicesData,
                          sizeof(CUSTOM_PROTOCOL),
                          (VOID **)&custom_protocol);
  if (EFI_ERROR(res))
  {
    return res;
  }

  custom_protocol->data = CUSTOM_PROTOCOL_DATA;

  res = uefi_call_wrapper(BS->InstallProtocolInterface, 4,
                          &ImageHandle,
                          &guid,
                          EFI_NATIVE_INTERFACE,
                          custom_protocol);
  if (EFI_ERROR(res))
  {
    uefi_call_wrapper(BS->FreePool, 1, custom_protocol);
  }

  return res;
}

CUSTOM_PROTOCOL *
uefi_get_custom_protocol()
{

  EFI_STATUS status = EFI_SUCCESS;
  EFI_GUID guid = EFI_CUSTOM_PROTOCOL_GUID;
  CUSTOM_PROTOCOL *custom_protocol = NULL;

  status = uefi_call_wrapper(BS->LocateProtocol, 3,
                             &guid, NULL, (void **)&custom_protocol);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to locate custom protocol: %d\n", status);
    return NULL;
  }

  return custom_protocol;
}