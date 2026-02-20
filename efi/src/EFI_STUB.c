#include "efi.h"
#include "efiapi.h"
#include "efidef.h"
#include "efierr.h"
#include "efiip.h"
#include "x86_64/efibind.h"
#include "x86_64/efilib.h"
#include <stdlib.h>

#define PTE_PRESENT (1ULL << 0)

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000ULL

typedef struct {
  UINT8 blue;
  UINT8 green;
  UINT8 red;
  UINT8 padding;
} Pixel;

typedef struct {
  UINTN MemoryMapSize;
  UINTN DescriptorSize;
  UINTN MapKey;
  UINT32 DescriptorVersion;
  EFI_PHYSICAL_ADDRESS MemoryMap;
} MEMORY_MAP_DESCRIPTOR;

typedef struct {
  Pixel *framebuffer;
  UINT64 framebuffersize;
  UINT32 VerticalResolution;
  UINT32 HorizontalResolution;
  UINT32 PixelsPerScanline;
  MEMORY_MAP_DESCRIPTOR *MemoryMapDescriptor;
} KERNEL_ARGS;

static EFI_PHYSICAL_ADDRESS KernelPhysicalAddress = 0;

static KERNEL_ARGS KernelArgsStruct;
static KERNEL_ARGS *KernelArgs = &KernelArgsStruct;

static EFI_PHYSICAL_ADDRESS *PageRoot = NULL;

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

CONST CHAR16 *EFIMemTypeToString(EFI_MEMORY_TYPE type) {
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

MEMORY_MAP_DESCRIPTOR *GetMemoryMap(EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  MEMORY_MAP_DESCRIPTOR *MemoryMapDescriptor = NULL;
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, sizeof(MEMORY_MAP_DESCRIPTOR),
                             &MemoryMapDescriptor);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to allocate pool for MEMORY_MAP_DESCRIPTOR: %s\n",
          EFIStatusToStr(status));
    return NULL;
  }

  MemoryMapDescriptor->MemoryMapSize = 0;

  status = uefi_call_wrapper(
      SystemTable->BootServices->GetMemoryMap, 5,
      &(MemoryMapDescriptor->MemoryMapSize), MemoryMapDescriptor->MemoryMap,
      &(MemoryMapDescriptor->MapKey), &(MemoryMapDescriptor->DescriptorSize),
      &(MemoryMapDescriptor->DescriptorVersion));
  if (status != EFI_BUFFER_TOO_SMALL) {
    Print(L"Failed to get size of memory map: %s\n", EFIStatusToStr(status));
    return NULL;
  }

  MemoryMapDescriptor->MemoryMapSize += 5 * MemoryMapDescriptor->DescriptorSize;
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, MemoryMapDescriptor->MemoryMapSize,
                             &(MemoryMapDescriptor->MemoryMap));
  if (status != EFI_SUCCESS) {
    Print(L"Failed to allocate memory for memory map: %s\n",
          EFIStatusToStr(status));
    return NULL;
  }

  status = uefi_call_wrapper(
      SystemTable->BootServices->GetMemoryMap, 5,
      &(MemoryMapDescriptor->MemoryMapSize), MemoryMapDescriptor->MemoryMap,
      &(MemoryMapDescriptor->MapKey), &(MemoryMapDescriptor->DescriptorSize),
      &(MemoryMapDescriptor->DescriptorVersion));
  if (status != EFI_SUCCESS) {
    Print(L"Failed to get memory map: %s\n", EFIStatusToStr(status));
    return NULL;
  }

  return MemoryMapDescriptor;
}

EFI_STATUS FreeMemoryMapDescriptor(MEMORY_MAP_DESCRIPTOR *MemoryMapDescriptor,
                                   EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  status = uefi_call_wrapper(SystemTable->BootServices->FreePool, 1,
                             MemoryMapDescriptor->MemoryMap);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to free memory map: %s\n", EFIStatusToStr(status));
    return status;
  }
  status = uefi_call_wrapper(SystemTable->BootServices->FreePool, 1,
                             MemoryMapDescriptor);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to free memory map descriptor: %s\n",
          EFIStatusToStr(status));
    return status;
  }
  return EFI_SUCCESS;
}

EFI_STATUS GetTable(UINT64 *table, UINTN entry, UINT64 **RetTable,
                    EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  if (table[entry] & PTE_PRESENT) {
    *RetTable = (UINT64 *)(table[entry] & PTE_ADDR_MASK);
    return EFI_SUCCESS;
  }
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4,
                             AllocateAnyPages, EfiLoaderData, 1, RetTable);
  if (status != EFI_SUCCESS) {
    return status;
  }
  for (UINTN i = 0; i < 0x1000; i++) {
    (*(UINT8 **)RetTable)[i] = 0;
  }
  table[entry] = (UINT64)*RetTable;
  return EFI_SUCCESS;
}

EFI_STATUS MMap(EFI_PHYSICAL_ADDRESS PHYS, EFI_VIRTUAL_ADDRESS VIRT,
                BOOLEAN Cached, UINT64 *PML4, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  EFI_PHYSICAL_ADDRESS phys;
  UINTN EntryPML4 = VIRT >> 39 & 0x1ff;
  UINTN EntryPDPT = VIRT >> 30 & 0x1ff;
  UINTN EntryPD = VIRT >> 21 & 0x1ff;
  UINTN EntryPT = VIRT >> 12 & 0x1ff;

  // Find / make PML4 entry
  UINT64 *PDPT;
  status = GetTable(PML4, EntryPML4, &PDPT, SystemTable);

  if (EFI_ERROR(status)) {
    Print(L"Failed to get PDPT: %s\n", EFIStatusToStr(status));
    return status;
  }
  PML4[EntryPML4] |= 3;

  // Find / make PDPT entry
  UINT64 *PD;
  status = GetTable(PDPT, EntryPDPT, &PD, SystemTable);

  if (EFI_ERROR(status)) {
    Print(L"Failed to get PD: %s\n", EFIStatusToStr(status));
    return status;
  }
  PDPT[EntryPDPT] |= 3;

  // Find/make PD entry
  UINT64 *PT;
  status = GetTable(PD, EntryPD, &PT, SystemTable);

  if (EFI_ERROR(status)) {
    Print(L"Failed to get PT: %s\n", EFIStatusToStr(status));
    return status;
  }
  PD[EntryPD] |= 3;

  // Make PT entry
  PT[EntryPT] = PHYS & ~(0xfff) | 0x3 | (Cached ? (1 << 4) : 0);

  return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  EFI_STATUS status;

  // Load info about EFI_STUB
  EFI_LOADED_IMAGE_PROTOCOL *RootImage;
  status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3,
                             ImageHandle, &gEfiLoadedImageProtocolGuid,
                             (VOID **)&RootImage);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to load EFI_STUB img: %s\n", EFIStatusToStr(status));
    return EFI_OUT_OF_RESOURCES;
  }

  // Load kernel bin
  // Open root filesystem
  EFI_GUID FileSysGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_FILE_IO_INTERFACE *RootVolumeInterface;
  EFI_FILE_HANDLE RootDir;
  status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3,
                             RootImage->DeviceHandle, &FileSysGuid,
                             &RootVolumeInterface);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to get Simple filesys protocol from RootImage: %s\n");
  }
  status = uefi_call_wrapper(RootVolumeInterface->OpenVolume, 2,
                             RootVolumeInterface, &RootDir);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to opem root volume: %s\n");
  }
  // Open kernel file
  EFI_FILE_HANDLE KernelHandle;
  status = uefi_call_wrapper(RootDir->Open, 5, RootDir, &KernelHandle,
                             L"kernel", EFI_FILE_MODE_READ, NULL);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to open kernel: %s\n", EFIStatusToStr(status));
  }
  // Get kernel file size
  EFI_FILE_INFO *KernelInfo = NULL;
  UINTN KernelInfoSize = 0;
  status = uefi_call_wrapper(KernelHandle->GetInfo, 4, KernelHandle,
                             &gEfiFileInfoGuid, &KernelInfoSize, KernelInfo);
  if (status != EFI_BUFFER_TOO_SMALL) {
    Print(L"Couldnt get size of kernel info buffer: %s\n",
          EFIStatusToStr(status));
  }
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                             EfiLoaderData, KernelInfoSize, &KernelInfo);
  if (status != EFI_SUCCESS) {
    Print(L"Couldnt allocate memory for kernel info buffer: %s\n",
          EFIStatusToStr(status));
  }
  status = uefi_call_wrapper(KernelHandle->GetInfo, 4, KernelHandle,
                             &gEfiFileInfoGuid, &KernelInfoSize, KernelInfo);
  if (status != EFI_SUCCESS) {
    Print(L"Couldnt get kernel info: %s\n", EFIStatusToStr(status));
  }
  // Allocate memory for kernel
  status = uefi_call_wrapper(
      SystemTable->BootServices->AllocatePages, 4, AllocateAnyPages,
      EfiLoaderData, KernelInfo->FileSize / 0x1000 + 1, &KernelPhysicalAddress);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to allocate memory for kernel: %s\n",
          EFIStatusToStr(status));
  }
  // Load the kernel to memory
  status = uefi_call_wrapper(KernelHandle->Read, 3, KernelHandle,
                             &(KernelInfo->FileSize), KernelPhysicalAddress);
  if (status != EFI_SUCCESS) {
    Print(L"Loading kernel failed: %s\n", EFIStatusToStr(status));
  }

  // Get GOP framebuffer info
  // Get GOP protocol
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GopProtocol = NULL;
  status =
      uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3,
                        &gEfiGraphicsOutputProtocolGuid, NULL, &GopProtocol);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to get GOP protocol: %s\n", EFIStatusToStr(status));
  }
  // Save framebuffer info
  KernelArgs->framebuffer = (Pixel *)GopProtocol->Mode->FrameBufferBase;
  KernelArgs->framebuffersize = GopProtocol->Mode->FrameBufferSize;
  KernelArgs->HorizontalResolution =
      GopProtocol->Mode->Info->HorizontalResolution;
  KernelArgs->VerticalResolution = GopProtocol->Mode->Info->VerticalResolution;
  KernelArgs->PixelsPerScanline = GopProtocol->Mode->Info->PixelsPerScanLine;

  // Init page map
  EFI_PHYSICAL_ADDRESS phys = 0;
  UINT64 *PML4 = NULL;
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4,
                             AllocateAnyPages, EfiLoaderData, 1, &phys);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to allocate memory for PML4: %s\n", EFIStatusToStr(status));
  }
  PML4 = (UINT64 *)phys;
  for (UINTN i = 0; i < 0x400; i++) {
    PML4[i] = 0;
  }

  // Allocate frame map
  MEMORY_MAP_DESCRIPTOR *InitMemoryMapDesc = GetMemoryMap(SystemTable);
  EFI_PHYSICAL_ADDRESS MaxPhysAddress = 0x8000000;
  for (UINTN i = 0; i < InitMemoryMapDesc->MemoryMapSize;
       i += InitMemoryMapDesc->DescriptorSize) {
    EFI_MEMORY_DESCRIPTOR *Entry =
        (EFI_MEMORY_DESCRIPTOR *)(InitMemoryMapDesc->MemoryMap + i);
    for (UINTN j = 0; j < Entry->NumberOfPages; j++) {

      MMap(Entry->PhysicalStart + j * 0x1000, Entry->PhysicalStart + j * 0x1000,
           Entry->Attribute & 0x1, PML4, SystemTable);
    }
  }

  // map framebuffer
  for (UINTN i = 0; i < GopProtocol->Mode->FrameBufferSize + 0x1000;
       i += 0x1000) {
    MMap(GopProtocol->Mode->FrameBufferBase + i, 0xffff800000000000 + i, FALSE,
         PML4, SystemTable);
  }
  KernelArgs->framebuffer = (Pixel *)0xffff800000000000;
  /*
    UINTN FramesOfMem = MaxPhysAddress >> 12;
    UINTN FrameMapSize = FramesOfMem / 8;
    EFI_PHYSICAL_ADDRESS FrameMap = 0;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4,
                               AllocateAnyPages, EfiLoaderData,
                               FrameMapSize >> 12, &FrameMap);
    if (status != EFI_SUCCESS) {
      Print(L"Failed to allocate memory for frame map: %s\n",
            EFIStatusToStr(status));
    }
    */
  // Init page map
  // EFI_PHYSICAL_ADDRESS phys = 0;
  // UINT64 *PML4 = NULL;
  // status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4,
  //                            AllocateAnyPages, EfiLoaderData, 1, &phys);
  // if (status != EFI_SUCCESS) {
  //   Print(L"Failed to allocate memory for PML4: %s\n",
  //   EFIStatusToStr(status));
  // }
  // PML4 = (UINT64 *)phys;
  // for (UINTN i = 0; i < 0x400; i++) {
  //   PML4[i] = 0;
  // }
  //
  //  for (UINTN i = 0; i < MaxPhysAddress; i += 0x1000) {
  //    MMap(i, i, TRUE, PML4, SystemTable);
  //  }
  /*
    // Remap kernel
    EFI_PHYSICAL_ADDRESS KernelPhysPage = KernelPhysicalAddress;
    EFI_VIRTUAL_ADDRESS KernelVirtPage = 0xffff800000000000;

    for (UINTN i = 0; i < KernelInfo->FileSize; i += 0x1000) {
      MMap(KernelPhysPage + i, KernelVirtPage + i, FALSE, PML4, SystemTable);
    }

  // idenity map EFI_STUB
  EFI_PHYSICAL_ADDRESS StubPhysPage =
      (EFI_PHYSICAL_ADDRESS)RootImage->ImageBase;
  EFI_VIRTUAL_ADDRESS StubVirtPage = (EFI_PHYSICAL_ADDRESS)RootImage->ImageBase;
  for (UINTN i = 0; i < RootImage->ImageSize; i += 0x1000) {
    MMap(StubPhysPage + i, StubVirtPage + i, TRUE, PML4, SystemTable);
  }

  // Remap the framebuffer
  EFI_VIRTUAL_ADDRESS FramebufferRemap = 0xffff800000000000;
  for (UINTN i = 0; i < KernelArgs->framebuffersize; i += 0x1000) {
    MMap((EFI_PHYSICAL_ADDRESS)KernelArgs->framebuffer + i,
         FramebufferRemap + i, FALSE, PML4, SystemTable);
  }

  // Map the stack
  EFI_PHYSICAL_ADDRESS stack = 0;
  status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4,
                             AllocateAnyPages, EfiLoaderData, 3, &stack);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to allocate memory for stack: %s\n", EFIStatusToStr(status));
  }
  for (UINTN i = 0x3000; i >= 0; i -= 0x1000) {
    MMap(stack + i, 0xffff900000000000 + i, TRUE, PML4, SystemTable);
  }
  */
  //  KernelArgs->framebuffer = (Pixel *)FramebufferRemap;
  KernelArgs->MemoryMapDescriptor = GetMemoryMap(SystemTable);

  status =
      uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
                        ImageHandle, KernelArgs->MemoryMapDescriptor->MapKey);
  if (status != EFI_SUCCESS) {
    Print(L"Failed to exit BootServices: %s\n", EFIStatusToStr(status));
  }
  asm volatile("movq %0, %%cr3" ::"r"(PML4) : "memory");
  /* asm volatile("mov %0, %%RSP\n"
                "mov %%RSP, %%RBP" ::"r"(0xffff900000003000)
                : "memory");
 */
  void (*kernel)();
  kernel = (void (*)())KernelPhysicalAddress;
  asm volatile("movq %0, %%rdi" ::"r"((UINT64)KernelArgs) : "memory");
  kernel();

  for (UINTN i = 0; i < KernelArgs->framebuffersize / 4; i++) {
    KernelArgs->framebuffer[i].red = 0xff;
  }
  while (TRUE) {
  }
  // Finish
  return EFI_SUCCESS;
}
