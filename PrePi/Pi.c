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
// #include <PreInitializeVariableInfo.h>


#define IS_XIP() (((UINT64)FixedPcdGet64 (PcdFdBaseAddress) > mSystemMemoryEnd) || \
                  ((FixedPcdGet64 (PcdFdBaseAddress) + FixedPcdGet32 (PcdFdSize)) < FixedPcdGet64 (PcdSystemMemoryBase)))

UINT64 mSystemMemoryEnd = FixedPcdGet64(PcdSystemMemoryBase) +
                          FixedPcdGet64(PcdSystemMemorySize) - 1;

STATIC VOID
UartInit
(
    VOID
)
{
    SerialPortInitialize();
    DEBUG ((EFI_D_ERROR, "\nTianoCore on Microsoft Lumia 535 (ARM)\n"));
    DEBUG ((EFI_D_ERROR,  "Firmware version %s built %a %a\n\n",
	        (CHAR16*) PcdGetPtr(PcdFirmwareVersionString),
			        __TIME__,
				__DATE__
	));

    DEBUG((
        EFI_D_INFO | EFI_D_LOAD,
        "SRC0_addrs at 0x%p\n",
        MmioRead32(0xFD901EA4)
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
    UINTN 			    UefiMemorySize = 0;

    // Initialize UART.
    UartInit();

    // Declare UEFI region
    MemoryBase      = FixedPcdGet32(PcdSystemMemoryBase);
    MemorySize      = FixedPcdGet32(PcdSystemMemorySize);
    UefiMemoryBase  = MemoryBase + FixedPcdGet32(PcdPreAllocatedMemorySize);
    UefiMemorySize  = FixedPcdGet32(PcdUefiMemPoolSize);
    StackBase       = (VOID*) (UefiMemoryBase + UefiMemorySize - StackSize);

    //Declear the PI/UEFI memory region
    HobList = HobConstructor (
    (VOID*)UefiMemoryBase,
    UefiMemorySize,
    (VOID*)UefiMemoryBase,
    StackBase  // The top of the UEFI Memory is reserved for the stacks
    );
    //PrePeiSetHobList (HobList);

    StackSize = FixedPcdGet32 (PcdCoreCount) * PcdGet32 (PcdCPUCorePrimaryStackSize);

    DEBUG((
        EFI_D_INFO | EFI_D_LOAD,
        "UEFI Memory Base = 0x%p, Size = 0x%p, Stack Base = 0x%p, Stack Size = 0x%p\n",
        UefiMemoryBase,
        UefiMemorySize,
        StackBase,
        StackSize
    ));
    PrePeiSetHobList(HobList);
    // Initialize MMU and Memory HOBs (Resource Descriptor HOBs)
    Status = MemoryPeim (UefiMemoryBase, UefiMemorySize);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Failed to configure MMU\n"));
        CpuDeadLoop();
    }else{
       DEBUG((EFI_D_INFO | EFI_D_LOAD, "MMU configured\n"));
    }

    // Initialize GIC
    Status = QGicPeim();
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "Failed to configure GIC\n"));
      CpuDeadLoop();
    }
    DEBUG((EFI_D_INFO | EFI_D_LOAD, "GIC configured\n"));


  // Create the Stacks HOB (reserve the memory for all stacks)	

  BuildStackHob ((UINTN)StackBase, StackSize);
  
  //TODO: Call CpuPei as a library
  BuildCpuHob (ArmGetPhysicalAddressBits (), PcdGet8 (PcdPrePiCpuIoSize));
  // Store timer value logged at the beginning of firmware image execution
  //Performance.ResetEnd = GetTimeInNanoSecond (StartTimeStamp);

  // Build SEC Performance Data Hob
  //BuildGuidDataHob (&gEfiFirmwarePerformanceGuid, &Performance, sizeof (Performance));

  // Set the Boot Mode
  SetBootMode (ArmPlatformGetBootMode ());

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
  PERF_START (NULL, "PEI", NULL, StartTimeStamp);

  // SEC phase needs to run library constructors by hand.
  ProcessLibraryConstructorList ();

  // Assume the FV that contains the SEC (our code) also contains a compressed FV.
  Status = DecompressFirstFv ();
  //ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "FV does not contains a compressed FV\n"));
    }else{
       DEBUG((EFI_D_INFO | EFI_D_LOAD, "FV contains a compressed FV\n"));
  }

  // Load the DXE Core and transfer control to it
  Status = LoadDxeCoreFromFv (NULL, 0);
  if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Failed to load DXE Core\n"));
    }else{
       DEBUG((EFI_D_INFO | EFI_D_LOAD, "Loading DXE Core\n"));
  }




    // We are done
    CpuDeadLoop();
}

VOID
CEntryPoint
(
    	IN  UINTN                     			  MpId,
	IN  VOID  					  *StackBase,
	IN  UINTN 					  StackSize
)
{
   UINT64 StartTimeStamp;
   ArmPlatformInitialize (MpId);

   if (ArmPlatformIsPrimaryCore (MpId) && PerformanceMeasurementEnabled ()) {
	       // Initialize the Timer Library to setup the Timer HW controller
		    TimerConstructor ();
		   // We cannot call yet the PerformanceLib because the HOB List has not been initialized
			StartTimeStamp = GetPerformanceCounter ();
   } else {
			StartTimeStamp = 0;
		  }

  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate Data cache
  ArmInvalidateDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

  // Define the Global Variable region when we are not running in XIP
  if (!IS_XIP()) {
    if (ArmPlatformIsPrimaryCore (MpId)) {
      if (ArmIsMpCore()) {
        // Signal the Global Variable Region is defined (event: ARM_CPU_EVENT_DEFAULT)
        ArmCallSEV ();
      }
    } else {
      // Wait the Primay core has defined the address of the Global Variable region (event: ARM_CPU_EVENT_DEFAULT)
      ArmCallWFE ();
    }
  }

   Main(StackBase, StackSize, StartTimeStamp);
}
