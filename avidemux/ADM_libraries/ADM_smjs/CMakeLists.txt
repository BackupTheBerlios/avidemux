SET(ADM_LIB ADM_smjs)

SET(${ADM_LIB}_SRCS 
	jsapi.c    jsatom.c  jscntxt.c   jsdbgapi.c  jsemit.c  jsfun.c   jsinterp.c  jslong.c  jsobj.c     jsprf.c     jsscope.c   jsutil.c    prmjtime.c
	jsarena.c  jsbool.c  jsdhash.c   jsexn.c   jsgc.c    jslock.c    jsmath.c  jsopcode.c  jsregexp.c  jsscript.c  jsxdrapi.c
	jsarray.c  jsdate.c    jsdtoa.c    jsfile.c  jshash.c  jslog2.c    jsnum.c   jsparse.c   jsscan.c    jsstr.c     jsxml.c)
	
ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})

IF (MINGW)
	ADD_TARGET_CFLAGS(${ADM_LIB} "-DXP_WIN")
ELSE (MINGW)
	ADD_TARGET_CFLAGS(${ADM_LIB} "-DXP_UNIX -DSVR4 -DSYSV -D_BSD_SOURCE -DPOSIX_SOURCE -DHAVE_VA_COPY -DVA_COPY=va_copy")
ENDIF (MINGW)

ADD_TARGET_CFLAGS(${ADM_LIB} "-DNDEBUG -DTRIMMED -DEXPORT_JS_API -DJS_USE_SAFE_ARENA -D_IEEE_LIBM")
ADD_TARGET_CFLAGS(${ADM_LIB} "-Dmalloc=ADM_alloc")
ADD_TARGET_CFLAGS(${ADM_LIB} "-Drealloc=ADM_realloc")
ADD_TARGET_CFLAGS(${ADM_LIB} "-Dfree=ADM_dezalloc")
ADD_TARGET_CFLAGS(${ADM_LIB} "-Dcalloc=ADM_calloc -I/usr/include/nspr")
ADD_TARGET_CFLAGS(${ADM_LIB} "-Dfopen=ADM_fopen")

TARGET_LINK_LIBRARIES(${ADM_LIB} ADM_core)

INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)