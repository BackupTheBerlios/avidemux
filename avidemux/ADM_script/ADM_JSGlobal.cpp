#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ADM_library/default.h"
#include "ADM_JSGlobal.h"
#include "ADM_JSAvidemux.h"

extern uint8_t JS_AvidemuxFunction(JSContext *cx,JSObject *global);
extern void A_Resync(void);
// expose our main javascript context to the entire program
JSObject *g_pObject;
JSContext *g_pCx;
JSRuntime *g_pRt;
JSClass g_globalClass =
{
	"Global", 0,
	JS_PropertyStub,  JS_PropertyStub,
	JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub,  JS_FinalizeStub
};

void
printJSError(JSContext *cx, const char *message, JSErrorReport *report)
{// begin printJSError
	printf("Spidermonkey ECMAScript Error: %s:%d:\n      %s\n",
		report->filename,
		report->lineno,
		message);
}// end printJSError

bool SpidermonkeyInit()
{// begin SpidermonkeyInit
	// setup JS
	g_pCx = NULL;
	g_pObject = NULL;
	g_pRt = NULL;
	JSRuntime *rt = JS_NewRuntime(1000000L);
	g_pRt = rt;
	if ( rt == NULL )
	{
		// Do some error reporting
		printf("Spidermonkey failed to initialize runtime!\n");
	}
	else
	{// begin runtime created
		JSContext *cx = JS_NewContext(rt, 8192);
		g_pCx = cx;
		if ( cx == NULL )
		{
			// Do some error reporting
			printf("Spidermonkey failed to initialize context!\n");
		}
		else
		{// begin context created

			JSObject *global = JS_NewObject(cx, &g_globalClass, 0, 0);
			g_pObject = global;
			JS_InitStandardClasses(cx, global);
			// load our custom JS class objects
			JSObject *obj = ADM_JSAvidemux::JSInit(cx, global);
			JSObject *objAudio = ADM_JSAvidemuxAudio::JSInit(cx, global);
//			JSObject *objVideo = ADM_JSAvidemuxVideo::JSInit(cx, global);
			// register error handler
			JS_SetErrorReporter(cx, printJSError);
                        JS_AvidemuxFunction(cx,global);
			return true;
		}// end context created
	}// end runtime created
	return false;
}// end SpidermonkeyInit

void SpidermonkeyDestroy()
{// begin SpidermonkeyDestroy
	JS_DestroyContext(g_pCx);
	JS_DestroyRuntime(g_pRt);
}// end SpidermonkeyDestroy

bool parseECMAScript(const char *name)
{// begin parseECMAScript
	jsval rval;
	uintN lineno = 0;
	printf("Spidermonkey compiling \"%s\"\n",name);
	JSScript *pJSScript = JS_CompileFile(g_pCx, g_pObject, name);
	if(pJSScript != NULL)
	{// begin execute external file
		printf("Spidermonkey executing \"%s\"\n",name);
		JSBool ok = JS_ExecuteScript(g_pCx, g_pObject, pJSScript, &rval);
		JS_DestroyScript(g_pCx,pJSScript);
	}// end execute external file
        A_Resync();
                        
}// end parseECMAScript
