#------------------------------------------------------------------------------
#
# Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
#
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#------------------------------------------------------------------------------

#include <AsmMacroIoLib.h>
#include <Library/PcdLib.h>
#include <Chipset/ArmV7.h>

.text
.align 3

#define LoadConstantToReg(Data, Reg) \
    ldr Reg, =Data
    
.global _StackBase 
.global _StackSize

_StackBase:
  .quad FixedPcdGet32(PcdPrePiStackBase)

_StackSize:
  .quad FixedPcdGet32(PcdPrePiStackSize)

GCC_ASM_IMPORT(CEntryPoint)
GCC_ASM_EXPORT(_ModuleEntryPoint)

ASM_PFX(_ModuleEntryPoint):  
  //Disable L2 cache
  mrc     p15, 0, r0, c1, c0, 1   // read Auxiliary Control Register
  bic     r0, r0, #0x00000002     // disable L2 cache
  mcr     p15, 0, r0, c1, c0, 1   // store Auxiliary Control Register

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
  ldr r0, _StackBase     /* Stack base arg0 */
  ldr r1, _StackSize     /* Stack size arg1 */
  
  add     r4, r0, r1
  add     sp, r4, #0

  //Enter SVC mode and set up SVC stack pointer
/*mov     r0,#0x13|0x80|0x40
  msr     CPSR_c,r0
  mov     r13,r4 */

_PrepareArguments:

	mov   r0, #0
	ldr   r1, _StackBase
	ldr   r2, _StackSize
	mov   r3, sp

  // Move sec startup address into a data register
  // Ensure we're jumping to FV version of the code (not boot remapped alias)
  ldr   r4, =ASM_PFX(CEntryPoint)

  // Jump to PrePiCore C code
  //    r0 = MpId
  //    r1 = UefiMemoryBase
  //    r2 = StacksBase
  blx   r4

ShouldNeverGetHere:
  /* _CEntryPoint should never return */
  b       ShouldNeverGetHere
