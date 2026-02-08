#include "efi.h"
#include "eficon.h"
#include "efidef.h"
#include "efierr.h"
#include "efiprot.h"
#include "x86_64/efibind.h"
#include "x86_64/efilib.h"
#include <cpuid.h>
#include <stddef.h>

typedef struct {
  UINT8 blue;
  UINT8 green;
  UINT8 red;
  UINT8 padding;
} Pixel;

static Pixel *framebuffer = NULL;
static UINT64 framebuffersize = 0;
static UINT32 VerticalResolution = 0;
static UINT32 HorizontalResolution = 0;
static UINT32 PixelsPerScanline = 0;
static EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;

EFI_FILE_HANDLE get_volume(EFI_HANDLE image) {
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

UINTN strcmp(const UINT8 *s1, const UINT8 *s2) {
  UINTN ret = 0;

  while (!(ret = *(UINT8 *)s1 - *(UINT8 *)s2) && *s2)
    ++s1, ++s2;

  if (ret < 0)

    ret = -1;
  else if (ret > 0)

    ret = 1;

  return ret;
}

EFI_STATUS getMemoryMap(EFI_SYSTEM_TABLE *SystemTable, UINTN *MemoryMapSize,
                        EFI_MEMORY_DESCRIPTOR **ppBuffer, UINTN *MapKey,
                        UINTN *DescriptorSize, UINT32 *DescriptorVersion) {
  *MemoryMapSize = 0;
  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
  EFI_STATUS status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap,
                                        5, MemoryMapSize, MemoryMap, MapKey,
                                        DescriptorSize, DescriptorVersion);
  if (status == EFI_INVALID_PARAMETER) {
    return EFI_INVALID_PARAMETER;
    Print(L"GetMemoryMap1 invalid parameter\n");
  }

  *MemoryMapSize += 5 * (*DescriptorSize);
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, *MemoryMapSize, &MemoryMap);
  if (status != EFI_SUCCESS) {
    if (status == EFI_OUT_OF_RESOURCES) {
      Print(L"AllocatePool out of resources\n");
    } else if (status == EFI_INVALID_PARAMETER) {
      Print(L"AllocatePool invalid paremeters\n");
    }
    return status;
  }
  status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                             MemoryMapSize, MemoryMap, MapKey, DescriptorSize,
                             DescriptorVersion);
  if (status != EFI_SUCCESS) {
    Print(L"GetMemoryMap2 failed\n");
    return status;
  }

  *ppBuffer = MemoryMap;
  return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  EFI_STATUS status;
  // Get GOP framebuffer
  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3,
                             &gopGuid, NULL, (void **)&gop);
  if (EFI_ERROR(status)) {
    Print(L"Unable to locate GOP\n");
  } else {
    Print(L"Located GOP\n");
  }
  framebuffer = (Pixel *)gop->Mode->FrameBufferBase;
  framebuffersize = gop->Mode->FrameBufferSize;
  VerticalResolution = gop->Mode->Info->VerticalResolution;
  HorizontalResolution = gop->Mode->Info->HorizontalResolution;
  PixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
  Print(L"HorizontalResolution: %llu\n", HorizontalResolution);

  // Load kernel
  EFI_FILE_HANDLE filehandle = get_volume(ImageHandle);
  EFI_FILE_INFO *rootinfo;
  UINTN bufferinfo = 1024;
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, bufferinfo, (void **)&rootinfo);
  do {
    status =
        uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, rootinfo);
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                               EfiLoaderData, bufferinfo, (void **)&rootinfo);

    status = uefi_call_wrapper(filehandle->Read, 4, filehandle, &bufferinfo,
                               rootinfo);
    if (status != EFI_SUCCESS) {
      if (status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Failed to get info about root %s\n", EFIStatusToStr(status));
      }
    } else {
      Print(L"%s\n", rootinfo->FileName);
    }
  } while (bufferinfo != 0 &&
           strcmp((UINT8 *)rootinfo->FileName, (UINT8 *)L"kernel"));

  Print(L"got info on file: %s\n", rootinfo->FileName);
  bufferinfo = rootinfo->FileSize;
  EFI_PHYSICAL_ADDRESS *filebuffer = (EFI_PHYSICAL_ADDRESS *)0x1000;
  EFI_FILE_PROTOCOL *fileinterface = NULL;
  status = uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, rootinfo);
  status = uefi_call_wrapper(
      SystemTable->BootServices->AllocatePages, 4, AllocateAddress,
      EfiLoaderData, bufferinfo / 0x1000 + 1, (EFI_PHYSICAL_ADDRESS)filebuffer);
  if (status != EFI_SUCCESS) {
    Print(L"Couldnt allocate pages for file %s\n", EFIStatusToStr(status));
  }
  status = uefi_call_wrapper(filehandle->Open, 5, filehandle, &fileinterface,
                             L"kernel", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
  if (status != EFI_SUCCESS) {
    Print(L"File couldnt open: %s\n", EFIStatusToStr(status));
  }
  status = uefi_call_wrapper(fileinterface->Read, 3, fileinterface, &bufferinfo,
                             filebuffer);
  if (status != EFI_SUCCESS) {
    Print(L"File couldnt read %s\n", EFIStatusToStr(status));
  }
  // get memory map
  EFI_STATUS Status;
  UINTN size, mapKey, retSize;
  EFI_MEMORY_DESCRIPTOR **pTopMemMap = &MemoryMap;
  UINT32 Version;
  EFI_INPUT_KEY key;
  status =
      getMemoryMap(SystemTable, &size, pTopMemMap, &mapKey, &retSize, &Version);
  if (status != EFI_SUCCESS) {
    Print(L"fuck\n");
  }
  EFI_MEMORY_DESCRIPTOR *MemMapBuffer = *pTopMemMap;
  status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
                             ImageHandle, mapKey);
  if (status != EFI_SUCCESS) {
    Print(L"Couldnt exit boot services %s\n", EFIStatusToStr(status));
  }

  asm volatile("movq %0, %%rsp;" ::"r"((UINT64)0xfff) : "memory");
  asm volatile("movq %%rsp, %%rbp" ::: "memory");

  void (*kernel)(Pixel *, UINT64, UINT32, UINT32, UINT32,
                 EFI_MEMORY_DESCRIPTOR *);
  kernel = (void (*)(Pixel *, UINT64, UINT32, UINT32, UINT32,
                     EFI_MEMORY_DESCRIPTOR *))0x1000;
  kernel(framebuffer, framebuffersize, VerticalResolution, HorizontalResolution,
         PixelsPerScanline, MemoryMap);

  for (UINTN i = 0; i < gop->Mode->FrameBufferSize / 4; i++) {
    framebuffer[i].red = 0xff;
  }
  // Finish
  while (1 == 1)
    ;

  return EFI_SUCCESS;
}
