####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_smjs.a

## INCLUDES were found outside kdevelop specific part

libADM_smjs_a_METASOURCES = AUTO

libADM_smjs_a_SOURCES = 		\
	jsapi.c		\
	jsarena.c	\
	jsarray.c	\
	jsatom.c	\
	jsbool.c	\
	jscntxt.c	\
	jsdate.c	\
	jsdbgapi.c	\
	jsdhash.c	\
	jsdtoa.c	\
	jsemit.c	\
	jsexn.c		\
	jsfun.c		\
	jsgc.c		\
	jshash.c	\
	jsinterp.c	\
	jslock.c	\
	jslog2.c	\
	jslong.c	\
	jsmath.c	\
	jsnum.c		\
	jsobj.c		\
	jsopcode.c      \
	jsparse.c	\
	jsprf.c		\
	jsregexp.c	\
	jsscan.c	\
	jsscope.c	\
	jsscript.c	\
	jsstr.c		\
	jsutil.c        \
	jsxdrapi.c	\
	jsxml.c		\
	prmjtime.c	


SUBDIRS = 

EXTRA_DIST = 

if HAVE_CYGWIN
INCLUDES = $(all_includes) -DXP_WIN
else
INCLUDES = $(all_includes) -DXP_UNIX -DSVR4 -DSYSV -D_BSD_SOURCE -DPOSIX_SOURCE -DHAVE_VA_COPY -DVA_COPY=va_copy
endif