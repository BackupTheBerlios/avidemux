//
// C++ Interface: ADM_cpuCap
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#if defined(ARCH_X86_64) ||defined(ARCH_X86)

#define CHECK_Z(x) {if(CpuCaps::myCpuCaps & ADM_CPU_##x) return 1; else return 0;} 

#else
#define         CHECK_Z(x) {return 0;}
#endif

typedef enum ADM_CPU_CAPS
{
        ADM_CPU_NONE   =1,
        ADM_CPU_MMX    =1<<1,
        ADM_CPU_MMXEXT =1<<2,
        ADM_CPU_3DNOW  =1<<3,
        ADM_CPU_SSE    =1<<4,
        ADM_CPU_SSE2   =1<<5,
        ADM_CPU_ALTIVEC=1<<6,
};

class CpuCaps
{
public:
        static int      myCpuCaps;
public:
	static void 	init( void);
	static uint8_t 	hasMMX (void) {CHECK_Z(MMX)};
	static uint8_t 	hasMMXEXT (void){CHECK_Z(MMXEXT)};
	static uint8_t 	has3DNOW (void) {CHECK_Z(3DNOW)};
	static uint8_t 	hasSSE (void) {CHECK_Z(SSE)};
	static uint8_t 	hasSSE2 (void){CHECK_Z(SSE2)};


};
uint32_t ADM_useNbThreads( void ); // FIXME, should not be here
#if (defined( ARCH_X86)  || defined(ARCH_X86_64)) && defined(HAVE_BUILTIN_VECTOR)
#define USE_SSE 1
#endif
