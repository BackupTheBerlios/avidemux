#include <stdio.h>
#include "default.h"
#include "libwrapper_global.h"

void initLibWrappers()
{
#ifdef USE_AMR_NB
	amrnb = new ADM_LibWrapperAmrnb();
#endif
#ifdef USE_LIBDCA
	dca = new ADM_LibWrapperDca();
#endif
}

void destroyLibWrappers()
{
#ifdef USE_AMR_NB
	delete amrnb;
#endif
#ifdef USE_LIBDCA
	delete dca;
#endif
}

#ifdef USE_AMR_NB
extern "C" ADM_LibWrapperAmrnb* getAmrnbWrapper(void)
{ 
	return amrnb;
}
#endif