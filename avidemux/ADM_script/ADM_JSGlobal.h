#ifndef _ADM_JSGLOBAL_H
#define _ADM_JSGLOBAL_H

// Spidermonkey
// HACK: Add this to configure later
#include <jsapi.h>

// javscript debugging helper
void printJSError(JSContext *cx, const char *message, JSErrorReport *report);
bool SpidermonkeyInit();
void SpidermonkeyDestroy();
bool parseECMAScript(const char *name);

#endif
