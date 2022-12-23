// Pi.c: Entry point for SEC(Security).

#include <PiPei.h>

#include <Pi/PiHob.h>
#include <Library/DebugLib.h>
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/ArmLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Library/DebugAgentLib.h>
#include <Ppi/GuidedSectionExtraction.h>
#include <Guid/LzmaDecompress.h>
#include <Library/SerialPortLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FrameBufferSerialPortLib.h>
#include <Library/TimerLib.h>
#include <Library/PerformanceLib.h>

#include <PiDxe.h>
#include "Pi.h"
STATIC VOID
UartInit
(
    VOID
)
{
    SerialPortInitialize();
    DEBUG ((EFI_D_ERROR, "\nTianoCore on Microsoft Lumia 520 (ARM)\n"));
    DEBUG ((EFI_D_ERROR,  "Firmware version %s built %a %a\n\n",
	        (CHAR16*) PcdGetPtr(PcdFirmwareVersionString),
			        __TIME__,
				__DATE__
	));

}

VOID
Main
(
  IN VOID	    *StackBase,
	IN UINTN    StackSize,
	IN UINT64   StartTimeStamp
)
{
	  EFI_HOB_HANDOFF_INFO_TABLE*   HobList;
    EFI_STATUS                    Status;

    UINTN                           MemoryBase = 0;
    UINTN                           MemorySize = 0;
    UINTN                           UefiMemoryBase = 0;
    UINTN 			                    UefiMemorySize = 0;

    // Initialize UART.
    UartInit();

   // Declare UEFI region
  MemoryBase     = FixedPcdGet32(PcdSystemMemoryBase);
  MemorySize     = FixedPcdGet32(PcdSystemMemorySize);
  UefiMemoryBase = FixedPcdGet32(PcdUefiMemPoolBase);
  UefiMemorySize = FixedPcdGet32(PcdUefiMemPoolSize);
  StackBase      = (VOID*) (UefiMemoryBase + UefiMemorySize - StackSize);

    //Declare the PI/UEFI memory region
    HobList = HobConstructor (
    (VOID*)UefiMemoryBase,
    UefiMemorySize,
    (VOID*)UefiMemoryBase,
    StackBase  // The top of the UEFI Memory is reserved for the stacks
    );
    PrePeiSetHobList (HobList);

    DEBUG((
        EFI_D_INFO | EFI_D_LOAD,
        "UEFI Memory Base = 0x%p, Size = 0x%p, Stack Base = 0x%p, Stack Size = 0x%p\n",
        UefiMemoryBase,
        UefiMemorySize,
        StackBase,
        StackSize
    ));

    // Initialize MMU and Memory HOBs (Resource Descriptor HOBs)
    Status = MemoryPeim (UefiMemoryBase, UefiMemorySize);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Failed to configure MMU\n"));
        CpuDeadLoop();
    }else{
       DEBUG((EFI_D_INFO | EFI_D_LOAD, "MMU configured\n"));
    }
  // Create the Stacks HOB (reserve the memory for all stacks)	
    StackSize = PcdGet32 (PcdCPUCorePrimaryStackSize) +
                 ((FixedPcdGet32 (PcdCoreCount) - 1) * FixedPcdGet32 (PcdCPUCoreSecondaryStackSize));

  BuildStackHob ((UINTN)StackBase, StackSize);
  DEBUG((EFI_D_INFO | EFI_D_LOAD, "Stacks allocated!\n"));
  
  //TODO: Call CpuPei as a library
  BuildCpuHob (ArmGetPhysicalAddressBits (), PcdGet8 (PcdPrePiCpuIoSize));

  // Store timer value logged at the beginning of firmware image execution
  // Performance.ResetEnd = GetTimeInNanoSecond (StartTimeStamp);

  // Build SEC Performance Data Hob
  // BuildGuidDataHob (&gEfiFirmwarePerformanceGuid, &Performance, sizeof (Performance));

  // Set the Boot Mode
  SetBootMode(BOOT_WITH_FULL_CONFIGURATION);

  // Initialize Platform HOBs (CpuHob and FvHob)
  Status = PlatformPeim ();
  //ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Failed to Initialize Platform HOBS\n"));
    }else{
       DEBUG((EFI_D_INFO | EFI_D_LOAD, "Platform HOBS Initialized\n"));
  }

  // Now, the HOB List has been initialized, we can register performance information
  // PERF_START (NULL, "PEI", NULL, StartTimeStamp);

  // SEC phase needs to run library constructors by hand.
  ProcessLibraryConstructorList ();

  // Assume the FV that contains the SEC (our code) also contains a compressed FV.
  Status = DecompressFirstFv ();
  ASSERT_EFI_ERROR (Status);

  // Load the DXE Core and transfer control to it
  Status = LoadDxeCoreFromFv (NULL, 0);
  ASSERT_EFI_ERROR (Status);

    // We are done
    CpuDeadLoop();
}

VOID
CEntryPoint
(
	IN  VOID  					 *StackBase,
	IN  UINTN 					  StackSize
)
{

  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate Data cache
  ArmInvalidateDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

   Main(StackBase, StackSize, 0);

  // DXE Core should always load and never return
  ASSERT (FALSE);
}