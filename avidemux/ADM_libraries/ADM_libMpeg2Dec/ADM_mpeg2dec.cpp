//
// C++ Implementation: ADM_cpuCap
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ADM_default.h"
#ifdef ADM_CPU_X86
#include "ADM_libraries/ADM_libMpeg2Dec/mpeg2_cpu.h"
#include "ADM_libraries/ADM_lavcodec/dsputil_cpu.h"
#include "ADM_cpuCap.h"
#endif
extern "C"
{
int ADM_mpeg2dec_mm_support(void);
}
int ADM_mpeg2dec_mm_support(void)
{
int rval=0;

#ifdef ADM_CPU_X86
#undef MATCH
#define MATCH(x,y) if(CpuCaps::myCpuCaps & CpuCaps::myCpuMask & ADM_CPUCAP_##x) rval|=MPEG2_ACCEL_X86_##x;
	
	MATCH(MMX,MMX);
	MATCH(MMXEXT,MMXEXT);
	MATCH(3DNOW,3DNOW);
#endif

	return rval;
}
