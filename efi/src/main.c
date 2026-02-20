#include "efi.h"
#include "efidef.h"
#include "efierr.h"
#include "efiprot.h"
#include "x86_64/efibind.h"
#include "x86_64/efilib.h"
#include <complex.h>
#include <cpuid.h>
#include <stddef.h>

#define KernelAddress 0xffff00000000

CONST CHAR16 *EFIStatusToStr(EFI_STATUS Status) {
  switch (Status) {
  case EFI_SUCCESS:
    return L"EFI_SUCCESS";
  case EFI_LOAD_ERROR:
    return L"EFI_LOAD_ERROR";
  case EFI_INVALID_PARAMETER:
    return L"EFI_INVALID_PARAMETER";
  case EFI_UNSUPPORTED:
    return L"EFI_UNSUPPORTED";
  case EFI_BAD_BUFFER_SIZE:
    return L"EFI_BAD_BUFFER_SIZE";
  case EFI_BUFFER_TOO_SMALL:
    return L"EFI_BUFFER_TOO_SMALL";
  case EFI_NOT_READY:
    return L"EFI_NOT_READY";
  case EFI_DEVICE_ERROR:
    return L"EFI_DEVICE_ERROR";
  case EFI_WRITE_PROTECTED:
    return L"EFI_WRITE_PROTECTED";
  case EFI_OUT_OF_RESOURCES:
    return L"EFI_OUT_OF_RESOURCES";
  case EFI_VOLUME_CORRUPTED:
    return L"EFI_VOLUME_CORRUPTED";
  case EFI_VOLUME_FULL:
    return L"EFI_VOLUME_FULL";
  case EFI_NO_MEDIA:
    return L"EFI_NO_MEDIA";
  case EFI_MEDIA_CHANGED:
    return L"EFI_MEDIA_CHANGED";
  case EFI_NOT_FOUND:
    return L"EFI_NOT_FOUND";
  case EFI_ACCESS_DENIED:
    return L"EFI_ACCESS_DENIED";
  case EFI_TIMEOUT:
    return L"EFI_TIMEOUT";
  case EFI_NOT_STARTED:
    return L"EFI_NOT_STARTED";
  case EFI_ALREADY_STARTED:
    return L"EFI_ALREADY_STARTED";
  case EFI_ABORTED:
    return L"EFI_ABORTED";
  case EFI_ICMP_ERROR:
    return L"EFI_ICMP_ERROR";
  case EFI_TFTP_ERROR:
    return L"EFI_TFTP_ERROR";
  case EFI_PROTOCOL_ERROR:
    return L"EFI_PROTOCOL_ERROR";
  default:
    return L"UNKNOWN_EFI_STATUS";
  }
}

const CHAR16 *EFIMemTypeToString(EFI_MEMORY_TYPE type) {
  switch (type) {
  case EfiReservedMemoryType:
    return L"EfiReservedMemoryType";
  case EfiLoaderCode:
    return L"EfiLoaderCode";
  case EfiLoaderData:
    return L"EfiLoaderData";
  case EfiBootServicesCode:
    return L"EfiBootServicesCode";
  case EfiBootServicesData:
    return L"EfiBootServicesData";
  case EfiRuntimeServicesCode:
    return L"EfiRuntimeServicesCode";
  case EfiRuntimeServicesData:
    return L"EfiRuntimeServicesData";
  case EfiConventionalMemory:
    return L"EfiConventionalMemory";
  case EfiUnusableMemory:
    return L"EfiUnusableMemory";
  case EfiACPIReclaimMemory:
    return L"EfiACPIReclaimMemory";
  case EfiACPIMemoryNVS:
    return L"EfiACPIMemoryNVS";
  case EfiMemoryMappedIO:
    return L"EfiMemoryMappedIO";
  case EfiMemoryMappedIOPortSpace:
    return L"EfiMemoryMappedIOPortSpace";
  case EfiPalCode:
    return L"EfiPalCode";
  case EfiPersistentMemory:
    return L"EfiPersistentMemory";
  case EfiUnacceptedMemoryType:
    return L"EfiUnacceptedMemoryType";
  case EfiMaxMemoryType:
    return L"EfiMaxMemoryType";
  default:
    return L"EfiInvalidMemoryType";
  }
}

EFI_FILE_HANDLE GET_VOLUME(EFI_HANDLE image) {
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
  return Volume;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  EFI_STATUS status;

  // getting memory map
  UINT32 DescriprtorVersion;
  UINTN MemoryMapSize = 0;
  UINTN MapKey, DescriptorSize;
  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;

  status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                             &MemoryMapSize, MemoryMap, &MapKey,
                             &DescriptorSize, &DescriprtorVersion);
  if (status != EFI_SUCCESS) {
    if (status != EFI_BUFFER_TOO_SMALL) {
      Print(L"Getting memory map failed\n");
      return EFI_INVALID_PARAMETER;
    }
  }
  MemoryMapSize += 100;

  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, MemoryMapSize, &MemoryMap);
  if (status != EFI_SUCCESS) {
    Print(L"Allocating memory for memory map faield because %s\n",
          EFIStatusToStr(status));
    return EFI_INVALID_PARAMETER;
  }

  status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                             &MemoryMapSize, MemoryMap, &MapKey,
                             &DescriptorSize, &DescriprtorVersion);
  if (status != EFI_SUCCESS) {
    Print(L"Getting memory map failed %s, %llu\n", EFIStatusToStr(status),
          MemoryMapSize);
    return EFI_INVALID_PARAMETER;
  }

  // Allocate space for kernel at 0xffff00000000
  EFI_FILE_HANDLE RootDirHandle = GET_VOLUME(ImageHandle);
  EFI_FILE_PROTOCOL *KernelHandle;
  UINTN KernelInfoSize = 1024;
  EFI_FILE_INFO KernelInfo[1024];

  status =
      uefi_call_wrapper(RootDirHandle->Open, 5, RootDirHandle, &KernelHandle,
                        L"kernel", EFI_FILE_MODE_READ, NULL);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to open kernel file because %s\n", EFIStatusToStr(status));
  }
  status = uefi_call_wrapper(KernelHandle->GetInfo, 4, KernelHandle,
                             &gEfiFileInfoGuid, &KernelInfoSize, KernelInfo);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to get kernel info because %s\n", EFIStatusToStr(status));
  }

  UINTN KernelReserveSize, ResereveFromMap, ReserveFromFile;
  EFI_PHYSICAL_ADDRESS *ReserveBuffer;
  for (UINTN i = 0; i < MemoryMapSize; i += DescriptorSize) {
    EFI_MEMORY_DESCRIPTOR *MemoryMapEntry =
        (EFI_MEMORY_DESCRIPTOR *)((VOID *)MemoryMap + i);
    if (MemoryMapEntry->PhysicalStart +
            MemoryMapEntry->NumberOfPages * 0x1000 >=
        KernelAddress) {
      if (MemoryMap->PhysicalStart <= KernelAddress) {
        if (MemoryMapEntry->Type == EfiConventionalMemory) {
          ReserveFromFile = KernelInfo->FileSize / 0x1000;
          ResereveFromMap =
              MemoryMapEntry->NumberOfPages -
              (KernelAddress - MemoryMapEntry->PhysicalStart) / 0x1000;
          if (ResereveFromMap > ReserveFromFile) {
            status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages,
                                       4, AllocateAddress, EfiLoaderData,
                                       ReserveFromFile, ReserveBuffer);
          } else {
            status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages,
                                       4, AllocateAddress, EfiLoaderData,
                                       ResereveFromMap, ReserveBuffer);
          }
          Print(L"Phys address %lllx\nPages count %lllu\n",
                MemoryMapEntry->PhysicalStart, MemoryMapEntry->NumberOfPages);
        }
      }
    }
  }

  // Loading the EFI_STUB.efi
  EFI_LOADED_IMAGE_PROTOCOL *RootImage;
  status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3,
                             ImageHandle, &gEfiLoadedImageProtocolGuid,
                             (VOID **)&RootImage);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to load device image: %s\n", EFIStatusToStr(status));
  }
  EFI_DEVICE_PATH_PROTOCOL *DevicePath = NULL;
  DevicePath = FileDevicePath(RootImage->DeviceHandle, L"\\EFI_STUB.efi");
  if (DevicePath == NULL) {
    Print(L"Failed to find file device pathn\n");
  }
  EFI_HANDLE *EfiStubHandle;
  status =
      uefi_call_wrapper(SystemTable->BootServices->LoadImage, 6, FALSE,
                        ImageHandle, DevicePath, NULL, NULL, EfiStubHandle);
  if (status != EFI_SUCCESS) {
    Print(L"Loading EFI_STUB.efi failed: %s\n", EFIStatusToStr(status));
  }

  status = uefi_call_wrapper(SystemTable->BootServices->StartImage, 3,
                             *EfiStubHandle, NULL, NULL);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to launch EFI_STUB: %s\n", EFIStatusToStr(status));
  }

  while (TRUE) {
  }

  return EFI_SUCCESS;
}
