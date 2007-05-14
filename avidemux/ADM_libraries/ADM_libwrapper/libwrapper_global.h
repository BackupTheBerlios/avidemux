#ifndef LIBWRAPPER_GLOBAL
#define LIBWRAPPER_GLOBAL

#include "config.h"

// AMRNB
#ifdef USE_AMR_NB
#define _TYPEDEF_H

#include "libwrapper_amrnb.h"

#if defined(__cplusplus)
extern ADM_LibWrapperAmrnb* amrnb
#if defined(__DECLARE__)
=NULL
#endif	// __DECLARE__
;
#else
struct ADM_LibWrapperAmrnb* getAmrnbWrapper(void);
#endif	// __cplusplus
#endif	// USE_AMR_NB

// LIBDCA
#if defined(USE_LIBDCA) && defined(__cplusplus)
#include "libwrapper_dca.h"

extern ADM_LibWrapperDca* dca
#ifdef __DECLARE__
=NULL
#endif
;
#endif

// funcs
void initLibWrappers();
void destroyLibWrappers();

#endif
