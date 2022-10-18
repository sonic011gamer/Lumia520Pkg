/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/qcom_lk.h>

#include <Library/qcom_msm8960_iomap.h>
#include <Library/qcom_msm8960_irqs.h>
#include <Library/qcom_msm8960_clock.h>
#include <Library/qcom_msm8960_timer.h>
#include <ArmGenericTimerCounterLib>

#ifdef MDE_CPU_ARM
#define MultU64xN MultU64x32
#else
#define MultU64xN MultU64x64
#endif

#define GPT_ENABLE_CLR_ON_MATCH_EN        2
#define GPT_ENABLE_EN                     1


//doc
//https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/arm/msm/timer.txt
//https://github.com/tsgan/qualcomm/blob/master/timer.c
//https://github.com/groeck/coreboot/blob/a234f45601e6e85a5179ec9cc446f070b86f425b/src/soc/qualcomm/ipq806x/timer.c



//ms�Ǻ���=0.001��
//us��΢��=0.000001��
//ns������=0.000000001��
//http://blog.163.com/arm_linux008/blog/static/13780414220107462045150/


//1��=1000����=1000000΢��=1000000000����=1000000000000Ƥ��=1000000000000000����
//1s=1000ms=1000000us=1000000000ns=1000000000000ps=1000000000000000fs
//�ں˲㣺
//   include 
//   1��void ndelay(unsigned long nsecs);         ���뼶��1/10^-10
//   2��void udelay(unsigned long usecs);         ΢�뼶: 1/10^-6
//   3��void mdelay(unsigned long msecs);         ���뼶��1/10^-3

//   * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
//   * its multiples. In Rumi GPT clock is 32 KHz


//ref 
// clock is 32768Hz = 33k
// 1ms = 33 ticks
// 1us = 0.033 ticks
// 1ns = 0.000033 ticks
//������ʱ
//     1000us        30us
//     33 ticks      1 ticks
void mdelay(unsigned msecs)
{
	if(1)
	{
		MicroSecondDelay(msecs);
	}
	else
	{
		msecs *= 33;

		writel(0, GPT_CLEAR);
		writel(0, GPT_ENABLE);
		while (readl(GPT_COUNT_VAL) != 0);

		writel(GPT_ENABLE_EN, GPT_ENABLE);
		while (readl(GPT_COUNT_VAL) < msecs);

		writel(0, GPT_ENABLE);
		writel(0, GPT_CLEAR);
	}
}

//΢����ʱ��С�ӳ���30us��usecs = 1
void udelay(unsigned usecs)
{

	if(1)
	{
		NanoSecondDelay(usecs*1000);
	}
	else
	{
		usecs = (usecs * 33 + 1000 - 33) / 1000;

		writel(0, GPT_CLEAR);
		writel(0, GPT_ENABLE);
		while (readl(GPT_COUNT_VAL) != 0);

		writel(GPT_ENABLE_EN, GPT_ENABLE);
		while (readl(GPT_COUNT_VAL) < usecs);

		writel(0, GPT_ENABLE);
		writel(0, GPT_CLEAR);
	}
}


RETURN_STATUS
EFIAPI
TimerConstructor (
  VOID
  )
{
	//DEBUG ((EFI_D_ERROR, "TimerLib:TimerConstructor\n"));
	return EFI_SUCCESS;
}

//����
UINTN
EFIAPI
MicroSecondDelay (
  IN  UINTN MicroSeconds
  )
{
	UINT64  NanoSeconds;
	//DEBUG ((EFI_D_ERROR, "TimerLib:MicroSecondDelay MicroSeconds=%d start\n",MicroSeconds));
	NanoSeconds = MultU64x32(MicroSeconds, 1000);

	while (NanoSeconds > (UINTN)-1) { 
		NanoSecondDelay((UINTN)-1);
		NanoSeconds -= (UINTN)-1;
	}
	
	NanoSecondDelay(NanoSeconds);
	//DEBUG ((EFI_D_ERROR, "TimerLib:MicroSecondDelay MicroSeconds=%d end\n",MicroSeconds));
	return MicroSeconds;
}


VOID Set_DGT_Enable(int en)
{
	UINT32 DATA = MmioRead32(DGT_ENABLE);
	UINT32 DATA_NEW;
	if(en)
	{
		DATA_NEW = DATA | (1 << 0);
	}
	else
	{
		DATA_NEW = DATA & ~(1 << 0);
	}
	MmioWrite32 (DGT_ENABLE, DATA_NEW);
}

VOID Set_DGT_ClrOnMatch(int en)
{
	UINT32 DATA = MmioRead32(DGT_ENABLE);
	UINT32 DATA_NEW;
	if(en)
	{
		DATA_NEW = DATA | (1 << 1);
	}
	else
	{
		DATA_NEW = DATA & ~(1 << 1);
	}
	MmioWrite32 (DGT_ENABLE, DATA_NEW);
}

#define GIC_DIST_REG(off)           (MSM_GIC_DIST_BASE + (off))
#define GIC_DIST_ENABLE_CLEAR       GIC_DIST_REG(0x180)

#define GIC_PPI_START 16
#define INT_DEBUG_TIMER_EXP     (GIC_PPI_START + 1)

//΢��
UINTN
EFIAPI
NanoSecondDelay (
  IN  UINTN NanoSeconds
  )
{
	//DEBUG ((EFI_D_ERROR, "TimerLib:NanoSecondDelay NanoSeconds=%d,use udelay start\n",NanoSeconds));
	
	UINTN ret = 0;
	{
		UINT32 vector = INT_DEBUG_TIMER_EXP;
		
		UINT32 reg = GIC_DIST_ENABLE_CLEAR + (vector / 32) * 4;
		UINT32 bit = 1U << (vector & 31);
		writel(bit, reg);
	}
	
	
	
	//if(NanoSeconds>=25)
	{
		Set_DGT_Enable(0);
		Set_DGT_ClrOnMatch(0);
		
		MmioWrite32(DGT_CLK_CTL, 3);
		MmioWrite32(DGT_MATCH_VAL,0);
		MmioWrite32(DGT_CLEAR,0);
		
		Set_DGT_Enable(1);
		
		
		
		int ticks_per_sec = 27000000 / 4;
		UINT64 AA = NanoSeconds * ticks_per_sec;
		UINT64 BB = AA / 1000000000;
		
		if(BB == 0)
		{
			BB = 25;
		}
		
		//UINT32 CNT2;
		//UINT32 CNT;
		
		//Get_DGT_CNT(&CNT2);
		//do
		//	Get_DGT_CNT(&CNT);
		//while ( (CNT - CNT2) < BB );
		UINT32 StartTime,CurrentTime,ElapsedTime;
		
		StartTime = MmioRead32 (DGT_COUNT_VAL);

		do 
		{
			CurrentTime = MmioRead32 (DGT_COUNT_VAL);
			ElapsedTime = CurrentTime - StartTime;
		} while (ElapsedTime < BB);
		
		ret =  ((ElapsedTime * 1000000000) / ticks_per_sec);
		
	}
	//else
	//{
	//	ret = NanoSeconds;
	//}
	//DEBUG ((EFI_D_ERROR, "TimerLib:NanoSecondDelay NanoSeconds=%d,use udelay end\n",NanoSeconds));
	
	return ret;
}

UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  // Just return the value of system count
  return ArmGenericTimerGetSystemCount ();
}

UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT UINT64  *StartValue,  OPTIONAL
  OUT UINT64  *EndValue     OPTIONAL
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:GetPerformanceCounterProperties need mpl!!!\n"));
	ASSERT(FALSE);
	if (StartValue != NULL) {
	// Timer starts with the reload value
	//*StartValue = (UINT64)MmioRead32 (TimerBase(PcdGet32(PcdOmap35xxFreeTimer)) + GPTIMER_TLDR);
	}

	if (EndValue != NULL) {
		// Timer counts up to 0xFFFFFFFF
		*EndValue = 0xFFFFFFFF;
	}
	return PcdGet64(PcdEmbeddedPerformanceCounterFrequencyInHz);
}

/**
  Converts elapsed ticks of performance counter to time in nanoseconds.
  This function converts the elapsed ticks of running performance counter to
  time value in unit of nanoseconds.
  @param  Ticks     The number of elapsed ticks of running performance counter.
  @return The elapsed time in nanoseconds.
**/
UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  NanoSeconds;
  UINT32  Remainder;
  UINT32  TimerFreq;

  TimerFreq = GetPlatformTimerFreq ();
  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64xN (
                  DivU64x32Remainder (
                    Ticks,
                    TimerFreq,
                    &Remainder),
                  1000000000U
                  );

  //
  // Frequency < 0x100000000, so Remainder < 0x100000000, then (Remainder * 1,000,000,000)
  // will not overflow 64-bit.
  //
  NanoSeconds += DivU64x32 (
                   MultU64xN (
                     (UINT64) Remainder,
                     1000000000U),
                   TimerFreq
                   );

  return NanoSeconds;
}