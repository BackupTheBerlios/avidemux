#ifndef LIBWRAPPER_GLOBAL
#define LIBWRAPPER_GLOBAL

#include "config.h"

#ifdef USE_LIBDCA
#include "libwrapper_dca.h"

extern ADM_LibWrapperDca* dca
#ifdef __DECLARE__
=NULL
#endif
;
#endif

void initLibWrappers();
void destroyLibWrappers();

#endif
