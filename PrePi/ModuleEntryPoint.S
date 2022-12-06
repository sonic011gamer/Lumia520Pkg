//------------------------------------------------------------------------------ 
//
// Copyright (c) 2010-2014, Qualcomm Technologies Inc. All rights reserved.
// Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//  
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

#include <AsmMacroIoLib.h>
#include <Library/PcdLib.h>

#define LoadConstantToReg(Data, Reg) \
    ldr Reg, =Data
.text
.align 3

.globl ASM_PFX(CEntryPoint)
GCC_ASM_EXPORT(_ModuleEntryPoint)
GCC_ASM_IMPORT (ArmEnableInstructionCache)
GCC_ASM_IMPORT (ArmEnableDataCache)
GCC_ASM_IMPORT (ArmInvalidateTlb)

ASM_PFX(_ModuleEntryPoint):

  //Disable L2 cache
  mrc     p15, 0, r0, c1, c0, 1   // read Auxiliary Control Register
  bic     r0, r0, #0x00000002     // disable L2 cache
  mcr     p15, 0, r0, c1, c0, 1   // store Auxiliary Control Register

  //Invalidate instruction cache
  bl      ArmInvalidateInstructionCache

    //Invalidate TLB
  bl      ArmInvalidateTlb

  //Enable Strict alignment checking & Instruction cache
  mrc     p15, 0, r0, c1, c0, 0
  bic     r0, r0, #0x00002300     /* clear bits 13, 9:8 (--V- --RS) */
  bic     r0, r0, #0x00000005     /* clear bits 0, 2 (---- -C-M) */
  //orr     r0, r0, #0x00000002     /* set bit 1 (A) Align */
  orr     r0, r0, #0x00001000     /* set bit 12 (I) enable I-Cache */
  mcr     p15, 0, r0, c1, c0, 0

  // Enable NEON register in case folks want to use them for optimizations (CopyMem)
  mrc     p15, 0, r0, c1, c0, 2
  orr     r0, r0, #0x00f00000   // Enable VPF access (V* instructions)
  mcr     p15, 0, r0, c1, c0, 2
  mov     r0, #0x40000000       // Set EN bit in FPEXC
  mcr     p10,#0x7,r0,c8,c0,#0  // msr     FPEXC,r0 in ARM assembly


  // Set CPU vectors to start of DRAM
  LoadConstantToReg (FixedPcdGet32(PcdCpuVectorBaseAddress) ,r0) // Get vector base
  mcr     p15, 0, r0, c12, c0, 0
  isb                               // Sync changes to control registers

  // Fill vector table with branchs to current pc (jmp $)
  ldr     r1, ShouldNeverGetHere
  movs    r2, #0
FillVectors:
  str     r1, [r0, r2]
  adds    r2, r2, #4
  cmp     r2, #32
  bne     FillVectors

  /* before we call C code, lets setup the stack pointer in internal RAM */
stack_pointer_setup:

  //
  // Set stack based on PCD values. Need to do it this way to make C code work
  // when it runs from FLASH.
  //
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackBase) ,r0)    /* stack base arg0  */
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackSize) ,r1)    /* stack size arg1  */
  add     r4, r0, r1

  //Enter SVC mode and set up SVC stack pointer
  mov     r0,#0x13|0x80|0x40
  msr     CPSR_c,r0
  mov     r13,r4

  //Invalidate data cache
  bl      ArmInvalidateDataCache
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackBase) ,r0)    // stack base arg0  
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackSize) ,r1)    // stack size arg1

  // Call C entry point
  blx      CEntryPoint /* Assume C code is thumb    */

ShouldNeverGetHere:
  /* _CEntryPoint should never return */
  b       ShouldNeverGetHere

