#include <efi.h>
#include <efilib.h>

#define KERNEL_IMAGE_NAME L"kernel.bin"
#define KERNEL_ENTRY_POINT 0x10000
typedef struct
{
  EFI_MEMORY_DESCRIPTOR *memory_map;
} kernel_boot_option_t;

typedef void (*kernel_main)(kernel_boot_option_t *options);

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

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_FILE_HANDLE volume;
  kernel_boot_option_t kernel_ops = {0};

  /* 1. Initialize system. */
  InitializeLib(ImageHandle, SystemTable);
  volume = uefi_get_volume(ImageHandle);

  status = uefi_load_kernel_image(volume, KERNEL_IMAGE_NAME, KERNEL_ENTRY_POINT);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to load kernel: %d\n", status);
    return status;
  }

  Print(L"Loaded kernel image.\n");

  /* 5. Loop to pause the boot progress. */
  while (1)
    ;

  ((kernel_main)KERNEL_ENTRY_POINT)(&kernel_ops);

  return EFI_SUCCESS;
}

EFI_FILE_HANDLE uefi_get_volume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;                  /* Image interface. */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* Image interface GUID. */
  EFI_FILE_IO_INTERFACE *IOVolume = NULL;                 /* File system interface. */
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* File system interface GUID. */
  EFI_FILE_HANDLE Volume;                                 /* The volume's interface. */

  /* 1. Get the loaded image protocol interface for our "image". */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **)&loaded_image);

  /* 2. Get the volume handle. */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID *)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);

  return Volume;
}

UINT64 uefi_get_file_size(EFI_FILE_HANDLE file_handle)
{
  UINT64 ret = 0;
  EFI_FILE_INFO *FileInfo = NULL;

  FileInfo = LibFileInfo(file_handle);

  ret = FileInfo->FileSize;
  FreePool(FileInfo);
  return ret;
}

EFI_STATUS uefi_open_file(EFI_FILE_HANDLE volume,
                          CHAR16 *filename,
                          EFI_FILE_HANDLE *file_handle)
{
  EFI_STATUS status = EFI_SUCCESS;
  status = uefi_call_wrapper(volume->Open, 5,
                             volume,
                             file_handle,
                             filename,
                             EFI_FILE_MODE_READ,
                             EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

  if (EFI_ERROR(status))
  {
    Print(L"Failed to open file: %d\n", status);
  }

  return status;
}

EFI_STATUS uefi_close_file(EFI_FILE_HANDLE file_handle)
{
  EFI_STATUS status = EFI_SUCCESS;
  status = uefi_call_wrapper(file_handle->Close, 1, file_handle);

  if (EFI_ERROR(status))
  {
    Print(L"Failed to close file: %d\n", status);
  }

  return status;
}

EFI_STATUS uefi_read_file(EFI_FILE_HANDLE file_handle,
                          UINT8 *buffer, UINT64 size)
{
  EFI_STATUS status = EFI_SUCCESS;
  UINT64 read_size = size;
  status = uefi_call_wrapper(file_handle->Read, 3, file_handle, &read_size, buffer);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to close file: %d\n", status);
  }
  else if (read_size != size)
  {
    Print(L"Can't get %d bytes, actual reading size: %d\n", size, read_size);
  }

  return status;
}

EFI_STATUS uefi_load_kernel_image(EFI_FILE_HANDLE volume,
                                  CHAR16 *filename,
                                  EFI_PHYSICAL_ADDRESS *kernel_entry_point)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_FILE_HANDLE file_handle;
  UINT64 file_size = 0;
  UINT8 *buffer = NULL;
  kernel_boot_option_t kernel_ops = {0};

  /* 1. Open the file. */
  status = uefi_open_file(volume, filename, &file_handle);
  if (EFI_ERROR(status))
  {
    goto exit;
  }

  /* 2. Get kernel filesize. */
  file_size = uefi_get_file_size(file_handle);
  Print(L"Kernel file size: %d, page: %d\n", file_size, EFI_SIZE_TO_PAGES(file_size));

  /* 3. Allocate physical pages for kernel. */
  status = uefi_call_wrapper(gBS->AllocatePages, 4,
                             AllocateAddress, EfiLoaderCode,
                             EFI_SIZE_TO_PAGES(file_size),
                             kernel_entry_point);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to allocate pages for kernel: %d\n", status);
    goto allocate_pages_failure;
  }

  /* 4. Allocate buffer to read kernel binary. */
  buffer = AllocatePool(file_size);

  /* 5. Read the kernel binary to buffer. */
  status = uefi_read_file(file_handle, buffer, file_size);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to read kernel binary: %d\n", status);
    goto uefi_read_file_failure;
  }

  /* 6. Copy from the buffer to kernel memory. */
  status = uefi_call_wrapper(gBS->CopyMem, 3,
                             kernel_entry_point, buffer, file_size);
  if (EFI_ERROR(status))
  {
    Print(L"Failed to copy kernel binary: %d\n", status);
    goto copy_mem_failure;
  }

  /* 7. Close the file. */
  goto exit_success;

copy_mem_failure:
uefi_read_file_failure:
  uefi_call_wrapper(gBS->FreePages, 2, kernel_entry_point, EFI_SIZE_TO_PAGES(file_size));
exit_success:
  FreePool(buffer);
allocate_pages_failure:
close_file:
  uefi_close_file(file_handle);
exit:
  return status;
}