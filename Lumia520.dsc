#
#  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.
#  Copyright (c) 2018 - 2019, RUIKAI LIU. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Lumia535
  PLATFORM_GUID                  = 566ba710-0bee-48e0-b56e-dcf4b0a38e04
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/Lumia535-$(ARCH)
  SUPPORTED_ARCHITECTURES        = ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Lumia535Pkg/Lumia535.fdf


[PcdsFixedAtBuild.common]
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x40000000         # 1GB
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000
  gArmPlatformTokenSpaceGuid.PcdCoreCount|4
  gLumia535PkgTokenSpaceGuid.PcdPreAllocatedMemorySize|0x3CD00000 #0FF00000
  gLumia535PkgTokenSpaceGuid.PcdUefiMemPoolSize|0x03300000

  # Default resolution for this firmware can be seen in dec file, override here for specific platform configuration if needed
  #gLumia535PkgTokenSpaceGuid.PcdMipiFrameBufferWidth|540
  #gLumia535PkgTokenSpaceGuid.PcdMipiFrameBufferHeight|960
  #gLumia535PkgTokenSpaceGuid.PcdMipiFrameBufferVisibleWidth|540
  #gLumia535PkgTokenSpaceGuid.PcdMipiFrameBufferVisibleHeight|960

[PcdsDynamicDefault.common]
  #
  # Set video resolution for boot options and for text setup.
  # PlatformDxe can set the former at runtime.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|540
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|960
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|540
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|960
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|120
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|50

  [PcdsPatchableInModule]
   # Console Resolution
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|540
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|960
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|50
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|120

!include Lumia535Pkg/Shared.dsc.inc
