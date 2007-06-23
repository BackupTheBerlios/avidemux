################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_smjs)
#
SET(${ADM_LIB}_SRCS 
jsapi.c    jsatom.c  jscntxt.c   jsdbgapi.c  jsemit.c  jsfun.c   jsinterp.c  jslong.c  jsobj.c     jsprf.c     jsscope.c   jsutil.c    prmjtime.c
jsarena.c  jsbool.c  jscpucfg.c  jsdhash.c   jsexn.c   jsgc.c    jslock.c    jsmath.c  jsopcode.c  jsregexp.c  jsscript.c  jsxdrapi.c
jsarray.c  jsdate.c    jsdtoa.c    jsfile.c  jshash.c  jslog2.c    jsnum.c   jsparse.c   jsscan.c    jsstr.c     jsxml.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
################################
IF(WIN32)
	add_definitions(-DXP_WIN)
ELSE(WIN32)
	add_definitions(-DXP_UNIX -DSVR4 -DSYSV -D_BSD_SOURCE -DPOSIX_SOURCE -DHAVE_VA_COPY -DVA_COPY=va_copy)
ENDIF(WIN32)

add_definitions(-DNDEBUG -DTRIMMED -DEXPORT_JS_API  -DJS_USE_SAFE_ARENA -D_IEEE_LIBMi)
#-DJS_THREADSAFE)
	add_definitions(-Dmalloc=ADM_alloc)
	add_definitions(-Drealloc=ADM_realloc)
	add_definitions(-Dfree=ADM_dezalloc)
	add_definitions(-Dcalloc=ADM_calloc -I/usr/include/nspr)
