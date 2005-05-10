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
#include "config.h"
#include <stdio.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/ADM_cpuCap.h"
#if defined( ARCH_X86)  || defined(ARCH_X86_64)
extern "C" 
{
#include "../adm_lavcodec/dsputil.h"
}
#endif


  void 	CpuCaps::init( void)
{
	printf("Checking cpu capabilities\n");
int probed=0;
#if defined( ARCH_X86)  || defined(ARCH_X86_64)
	probed=mm_support();
#define CHECK(x) if(probed & MM_##x) { CpuCaps::myCpuCaps|=ADM_CPU_##x;printf("\tCpu has "#x"\n");}
	CHECK(MMX);
	CHECK(3DNOW);
	CHECK(MMXEXT);
	CHECK(SSE);
	CHECK(SSE2);	
#endif	
	printf("End of cpu capabilities check\n");
}
