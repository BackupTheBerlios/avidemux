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

class CpuCaps
{
public:
	static void 	init( void);
	static uint8_t 	hasMMX (void);
	static uint8_t 	hasMMXEXT (void);
	static uint8_t 	has3DNOW (void);
	static uint8_t 	hasSSE (void);
	static uint8_t 	hasSSE2 (void);


};
#if defined(USE_MMX) && defined(HAVE_BUILTIN_VECTOR)
#define USE_SSE 1
#endif
