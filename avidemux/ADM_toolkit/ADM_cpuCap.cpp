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
extern "C"
{
	#include "adm_lavcodec/dsputil.h"
}
#include "ADM_toolkit/ADM_cpuCap.h"

static int myCpuCaps=0;

  void 	CpuCaps::init( void)
{
	printf("Checking cpu capabilities\n");
#ifdef HAVE_MMX
	myCpuCaps=mm_support();
#define CHECK(x) if(myCpuCaps & MM_##x) printf("Cpu has "#x"\n");
	CHECK(MMX);
	CHECK(3DNOW);
	CHECK(MMXEXT);
	CHECK(SSE);
	CHECK(SSE2);	
#endif	
	printf("End of cpu capabilities check\n");
}
  uint8_t 	CpuCaps::hasMMX (void)
{
uint8_t r=0;
#ifdef HAVE_MMX
	if(myCpuCaps & MM_MMX) r=1;
#endif
	return r;
}
  uint8_t 	CpuCaps::hasMMXEXT (void)
{
uint8_t r=0;
#ifdef HAVE_MMX
	if(myCpuCaps & MM_MMXEXT) r=1;
#endif
	return r;
}
  uint8_t 	CpuCaps::has3DNOW (void)
{
uint8_t r=0;
#ifdef HAVE_MMX
	if(myCpuCaps & MM_3DNOW) r=1;
#endif
	return r;

}
  uint8_t 	CpuCaps::hasSSE (void)
{
uint8_t r=0;
#if defined( USE_SSE)
	if(myCpuCaps & MM_SSE) r=1;
#endif
	return r;

}
  uint8_t 	CpuCaps::hasSSE2 (void)
{
uint8_t r=0;
#if defined( USE_SSE)
	if(myCpuCaps & MM_SSE2) r=1;
#endif
	return r;
}

