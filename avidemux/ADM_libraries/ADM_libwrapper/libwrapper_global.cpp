#include <stdio.h>
#include <stdlib.h>
#include "libwrapper_global.h"

void initLibWrappers()
{
#ifdef USE_LIBDCA
	dca = new ADM_LibWrapperDca();
#endif
}

void destroyLibWrappers()
{
#ifdef USE_LIBDCA
	delete dca;
#endif
}
