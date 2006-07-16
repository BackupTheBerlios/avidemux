#ifndef _ADM_JSGLOBAL_H
#define _ADM_JSGLOBAL_H

// Spidermonkey

#include <jsapi.h>
#include <pthread.h>

// javscript debugging helper
void printJSError(JSContext *cx, const char *message, JSErrorReport *report);
bool SpidermonkeyInit();
void SpidermonkeyDestroy();
bool parseECMAScript(const char *name);
void JS_setSuccess(bool bSuccess);
void *StartThreadSpidermonkey(void *pData);

#ifdef JSDECLARE
#define JSVAR(a,b,c) a b=c
#else
#define JSVAR(a,b,c) extern a b
#endif

JSVAR( pthread_t, g_pThreadSpidermonkey , 0);
JSVAR( pthread_mutex_t, g_pSpiderMonkeyMutex , PTHREAD_MUTEX_INITIALIZER);
// expose our main javascript context to the entire program
JSVAR( bool, g_bJSSuccess , 0);
JSVAR( JSObject, *g_pObject,NULL);
JSVAR( JSContext, *g_pCx,NULL);
JSVAR( JSRuntime, *g_pRt,NULL);


JSVAR( JSClass, g_globalClass,)
#ifdef JSDECLARE
{ 
	"Global", 0, 
	JS_PropertyStub,  JS_PropertyStub, 
	JS_PropertyStub, JS_PropertyStub, 
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub,  JS_FinalizeStub }
#endif
	;

#endif
//EOF

