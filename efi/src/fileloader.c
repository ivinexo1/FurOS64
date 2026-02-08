#include "efi.h"
#include "efierr.h"
#include "x86_64/efilib.h"
EFI_STATUS get_volume(EFI_HANDLE image, EFI_FILE_HANDLE *volumehandle) {
  EFI_LOADED_IMAGE *loaded_image = NULL;             /* image interface */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID; /* image interface GUID */
  EFI_FILE_IO_INTERFACE *IOVolume;                   /* file system interface */
  EFI_GUID fsGuid =
      EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
  EFI_FILE_HANDLE Volume;                   /* the volume's interface */

  /* get the loaded image protocol interface for our "image" */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid,
                    (void **)&loaded_image);
  /* get the volume handle */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid,
                    (VOID *)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
  *volumehandle = Volume;
  return EFI_SUCCESS;
}
