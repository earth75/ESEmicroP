/*********************************************************************************

Copyright(c) 2004 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

$RCSfile: services.h,v $
$Revision: 1.14 $
$Date: 2007/07/26 22:10:20 $

Description:
            This is the include file for the System Services Library

*********************************************************************************/

#ifndef __SERVICES_H__
#define __SERVICES_H__

/*********************************************************************

Identify processor core by family

*********************************************************************/

#if defined(__ADSPBF531__) || defined(__ADSPBF532__) || defined(__ADSPBF533__)

#ifndef __ADSP_EDINBURGH__
#define __ADSP_EDINBURGH__
#endif

#elif defined(__ADSPBF534__) || defined(__ADSPBF536__) || defined(__ADSPBF537__)
#ifndef __ADSP_BRAEMAR__
#define __ADSP_BRAEMAR__
#endif

#elif defined(__ADSPBF538__) || defined(__ADSPBF539__)

#ifndef __ADSP_STIRLING__
#define __ADSP_STIRLING__
#endif

#elif defined(__ADSPBF542__) || defined(__ADSPBF544__) || defined(__ADSPBF548__) || defined(__ADSPBF549__)

#ifndef __ADSP_MOAB__
#define __ADSP_MOAB__
#endif

#elif defined(__ADSPBF522__) || defined(__ADSPBF525__) || defined(__ADSPBF527__)

#ifndef __ADSP_KOOKABURRA__
#define __ADSP_KOOKABURRA__
#endif

#elif defined(__ADSPBF566__)

#ifndef __ADSP_EMERALD__
#define __ADSP_EMERALD__
#endif

#elif defined(__ADSPBF561__)

#ifndef __ADSP_TETON__
#define __ADSP_TETON__
#endif

#endif

/*********************************************************************

Insure the services support the given processor family

*********************************************************************/

#if defined(__ADSP_EDINBURGH__) || defined(__ADSP_BRAEMAR__) || defined(__ADSP_TETON__) || defined(__ADSP_STIRLING__) || defined(__ADSP_MOAB__) || defined(__ADSP_KOOKABURRA__)


#else
#error "*** System Services do not yet support this processor ***"
#endif


/*********************************************************************

Set up typedefs and common use macros

*********************************************************************/

// integer typedefs - for all modules
#if !defined(_LANGUAGE_ASM)

typedef unsigned short u16;
typedef unsigned long  u32;
typedef unsigned long long u64;
typedef unsigned char  u8;
typedef short s16;
typedef long  s32;
typedef long long s64;
typedef char  s8;

#endif

// TRUE, FALSE and NULL

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#if !defined(_LANGUAGE_ASM)

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif


/*********************************************************************

create macros to determine which core we're on for Teton

*********************************************************************/

#if defined(__ADSP_TETON__)

#define ADI_COREA (0xFF800000)
#define ADI_COREB (0xFFF00000)
#if !defined(_LANGUAGE_ASM)
#define ADI_SRAM_BASE_ADDR_MMR ((u32 *)0xFFE00000)
#else
#define ADI_SRAM_BASE_ADDR_MMR (0xFFE00000)
#endif
#endif




#if !defined(_LANGUAGE_ASM)

#include <sys/exception.h>          // interrupt handler defines
#include <stddef.h>

#endif

#if defined(__ECC__) || defined(_LANGUAGE_ASM)

#if defined(__ADSP_EDINBURGH__)
#if defined(_LANGUAGE_ASM)
#include <defBF532.h>
#else
#include <cdefBF532.h>
#include <ccblkfn.h>
#endif

#elif defined(__ADSP_BRAEMAR__)
#if defined(_LANGUAGE_ASM)
#include <defBF537.h>
#else
#include <cdefBF537.h>
#include <ccblkfn.h>
#endif

#elif defined(__ADSP_STIRLING__)
#if defined(_LANGUAGE_ASM)
#if defined(__ADSPBF538__)
#include <defBF538.h>
#endif
#if defined(__ADSPBF539__)
#include <defBF539.h>
#endif
#else   // end of assembly
#if defined(__ADSPBF538__)
#include <cdefBF538.h>
#endif
#if defined(__ADSPBF539__)
#include <cdefBF539.h>
#endif
#include <ccblkfn.h>
#endif

#elif defined(__ADSP_MOAB__)
#if defined(_LANGUAGE_ASM)
#if defined(__ADSPBF542__)
#include <defBF542.h>
#endif
#if defined(__ADSPBF544__)
#include <defBF544.h>
#endif
#if defined(__ADSPBF548__)
#include <defBF548.h>
#endif
#if defined(__ADSPBF549__)
#include <defBF549.h>
#endif
#else
#if defined(__ADSPBF542__)
#include <cdefBF542.h>
#endif
#if defined(__ADSPBF544__)
#include <cdefBF544.h>
#endif
#if defined(__ADSPBF548__)
#include <cdefBF548.h>
#endif
#if defined(__ADSPBF549__)
#include <cdefBF549.h>
#endif
#include <ccblkfn.h>
#endif

#elif defined(__ADSP_KOOKABURRA__)
#if defined(_LANGUAGE_ASM)
#if defined(__ADSPBF522__)
#include <defBF522.h>
#endif
#if defined(__ADSPBF525__)
#include <defBF525.h>
#endif
#if defined(__ADSPBF527__)
#include <defBF527.h>
#endif
#else
#if defined(__ADSPBF522__)
#include <cdefBF522.h>
#endif
#if defined(__ADSPBF525__)
#include <cdefBF525.h>
#endif
#if defined(__ADSPBF527__)
#include <cdefBF527.h>
#endif
#include <ccblkfn.h>
#endif

#elif defined(__ADSPBF561__)
#if defined(_LANGUAGE_ASM)
#include <defBF561.h>
#else
#include <cdefBF561.h>
#include <ccblkfn.h>
#endif
#endif

#elif defined(__ghs__)
#include <bf/cdefBF533.h>
#include <bf/ccblkfn.h>
// macros for certain ops
#define idle()  asm("IDLE;")
#define ssync() asm("SSYNC;")
#define csync() asm("CSYNC;")
#elif defined(__GNUC__)
#include <cdefBF533.h>
#include <ccblkfn.h>
#else
#error "System Services Library is not supported for this compiler"
#endif


/*********************************************************************

Define the starting points for enumerations within the services.  This
insures that the enumeration values for each of the services do not
overlap with one another.  This only applies to items such as command IDs,
event IDs, and return codes.  Note that the return code from each service
for generic success is always 0, while the return code from each service
for generic failure is always 1, regardless of the enumeration starting
point.

*********************************************************************/

#define ADI_DEV_ENUMERATION_START   (0x40000000)

#define ADI_DCB_ENUMERATION_START   (0x00020000)
#define ADI_DMA_ENUMERATION_START   (0x00030000)
#define ADI_EBIU_ENUMERATION_START  (0x00040000)
#define ADI_INT_ENUMERATION_START   (0x00050000)
#define ADI_PWR_ENUMERATION_START   (0x00060000)
#define ADI_TMR_ENUMERATION_START   (0x00070000)
#define ADI_FLAG_ENUMERATION_START  (0x00080000)
#define ADI_PORTS_ENUMERATION_START (0x00090000)
#define ADI_RTC_ENUMERATION_START   (0x000a0000)
#define ADI_FSS_ENUMERATION_START   (0x000b0000)
#define ADI_SEM_ENUMERATION_START   (0x000c0000)

/*********************************************************************

Pull in the individual include files.

*********************************************************************/

#include <services/int/adi_int.h>
#include <services/dcb/adi_dcb.h>
#include <services/dma/adi_dma.h>
#include <services/ebiu/adi_ebiu.h>
#include <services/pwr/adi_pwr.h>
#include <services/tmr/adi_tmr.h>
#include <services/rtc/adi_rtc.h>
#include <services/flag/adi_flag.h>
#include <services/ports/adi_ports.h>
#include <services/sem/adi_sem.h>


#endif // __SERVICES_H__
