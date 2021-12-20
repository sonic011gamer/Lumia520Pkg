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
  PLATFORM_NAME                  = Lumia520
  PLATFORM_GUID                  = 566ba710-0bee-48e0-b56e-dcf4b0a38e04
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/Lumia520-$(ARCH)
  SUPPORTED_ARCHITECTURES        = ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Lumia520Pkg/Lumia520.fdf


[PcdsFixedAtBuild.common]
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x20000000         # 512MB
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000
  gArmPlatformTokenSpaceGuid.PcdCoreCount|2
  gLumia520PkgTokenSpaceGuid.PcdPreAllocatedMemorySize|0x1CD00000 #0FF00000
  gLumia520PkgTokenSpaceGuid.PcdUefiMemPoolSize|0x03300000

  # Default resolution for this firmware can be seen in dec file, override here for specific platform configuration if needed
  #gLumia520PkgTokenSpaceGuid.PcdMipiFrameBufferWidth|480
  #gLumia520PkgTokenSpaceGuid.PcdMipiFrameBufferHeight|800
  #gLumia520PkgTokenSpaceGuid.PcdMipiFrameBufferVisibleWidth|480
  #gLumia520PkgTokenSpaceGuid.PcdMipiFrameBufferVisibleHeight|800

[PcdsDynamicDefault.common]
  #
  # Set video resolution for boot options and for text setup.
  # PlatformDxe can set the former at runtime.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|480
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|800
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|480
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|800
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|42
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|25

  [PcdsPatchableInModule]
   # Console Resolution
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|480
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|800
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|25
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|42

!include Lumia520Pkg/Shared.dsc.inc