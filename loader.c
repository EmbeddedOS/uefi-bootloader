#include <efi.h>
#include <efilib.h>

EFI_FILE_HANDLE uefi_get_volume(EFI_HANDLE image);

UINT64 uefi_get_file_size(EFI_FILE_HANDLE file_handle);

EFI_STATUS uefi_open_file(EFI_FILE_HANDLE volume,
                          CHAR16 *filename,
                          EFI_FILE_HANDLE *file_handle);

EFI_STATUS uefi_close_file(EFI_FILE_HANDLE file_handle);

EFI_STATUS uefi_read_file(EFI_FILE_HANDLE file_handle, UINT8 *buffer, UINT64 size);

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_FILE_HANDLE volume;
  EFI_FILE_HANDLE file_handle;
  UINT64 file_size = 0;
  UINT8 *buffer = NULL;

  /* 1. Initialize system. */
  InitializeLib(ImageHandle, SystemTable);
  volume = uefi_get_volume(ImageHandle);

  /* 2. Open the file. */
  status = uefi_open_file(volume, L"DATA.TXT", &file_handle);
  if (EFI_ERROR(status))
  {
    return status;
  }

  /* 3. Read from the file. */
  file_size = uefi_get_file_size(file_handle);
  buffer = AllocatePool(file_size);
  status = uefi_read_file(file_handle, buffer, file_size);
  if (EFI_ERROR(status))
  {
    return status;
  }

  Print(L"Read from file: %a\n", buffer);

  /* 4. Close the file. */
  status = uefi_close_file(file_handle);
  if (EFI_ERROR(status))
  {
    return status;
  }

  /* 5. Loop to pause the boot progress. */
  while (1)
    ;

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
