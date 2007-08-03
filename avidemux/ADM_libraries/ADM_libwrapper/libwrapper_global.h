#ifndef LIBWRAPPER_GLOBAL
#define LIBWRAPPER_GLOBAL

#include "config.h"
//
#if defined(__DECLARE__)
#define ZZ_PREFIX
#define ZZ_POSTFIX =NULL
#else
#define ZZ_PREFIX extern
#define ZZ_POSTFIX 
#endif //__DECLARE__

// AMRNB
#ifdef USE_AMR_NB
#define _TYPEDEF_H

#include "libwrapper_amrnb.h"

#if defined(__cplusplus)
ZZ_PREFIX ADM_LibWrapperAmrnb  *amrnb ZZ_POSTFIX;
#else
struct ADM_LibWrapperAmrnb* getAmrnbWrapper(void);
#endif	// __cplusplus
#endif	// USE_AMR_NB

// LIBDCA
#if defined(USE_LIBDCA) && defined(__cplusplus)
#include "libwrapper_dca.h"

ZZ_PREFIX ADM_LibWrapperDca* dca ZZ_POSTFIX;

// funcs
void initLibWrappers();
void destroyLibWrappers();
#endif // USE LIBDCA
#endif // LIBWRAPPER
