// C++ Interface: Spider Monkey interface
//
// Description: 
//
//
// Author: Anish Mistry
//      Some modification by mean
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#include <stdlib.h>
#include <math.h>
#include "ADM_JSAvidemux.h"
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_editor/ADM_outputfmt.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "adm_scanner.h" 
#include "avi_vars.h"
#include "gui_action.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_editor/ADM_outputfmt.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_filter/video_filters.h"
#include "ADM_script/ADM_container.h"

#include "ADM_JSGlobal.h"
#include "ADM_toolkit/filesel.h"

JSBool displayError(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool displayInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool fileWriteSelect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool fileReadSelect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec adm_functions[] = {
  /*    name          native          nargs    */
  {"displayError",      displayError,         1},
  {"displayInfo",       displayInfo,        1},
  {"fileReadSelect",    fileReadSelect,        0},
  {"fileWriteSelect",   fileWriteSelect,        0},
  {"print",             print,        0},
  {0}
};
uint8_t JS_AvidemuxFunction(JSContext *cx,JSObject *global)
{
        if( JS_DefineFunctions(cx, global, adm_functions)==true) return 1;
        
        printf("Error in JSAvidemuxfunction\n");
        return 0;
}
JSBool displayError(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        // default return value
        if(argc != 1)
                return JS_FALSE;
        char  *stringa = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        GUI_Verbose();
        GUI_Alert(stringa);
        GUI_Quiet();
        
        return JS_TRUE;
}// end AddSegment
JSBool displayInfo(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        // default return value
        if(argc != 1)
                return JS_FALSE;
        char  *stringa = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        GUI_Verbose();
        GUI_Info(stringa);
        GUI_Quiet();
        return JS_TRUE;
}// end AddSegment

JSBool fileReadSelect(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        char *name;
        // default return value
        if(argc != 0)
                return JS_FALSE;
        if(!name) return JS_FALSE;
        GUI_FileSelRead("Open file (Read mode)", &name);
        *rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx,name));
        ADM_dealloc(name);
        return JS_TRUE;
}// end AddSegment
JSBool fileWriteSelect(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        char *name;
        // default return value
        if(argc != 0)
                return JS_FALSE;
        GUI_FileSelWrite("Open file (Write mode)", &name);
        if(!name) return JS_FALSE;
        *rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx,name));
        ADM_dealloc(name);
        return JS_TRUE;
}// end AddSegment
JSBool print(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        char *str;
        ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        // default return value
        if(argc != 1)
                return JS_FALSE;
        str=JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        fprintf(stderr,"JSConsole:%s\n",str);
        return JS_TRUE;
}// end AddSegment
