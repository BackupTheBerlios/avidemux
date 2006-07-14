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
#include <dirent.h>
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

extern int JS_setSuccess(int a);;
JSBool displayError(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool displayInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool fileWriteSelect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool fileReadSelect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool allFilesFrom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool nextFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool setSuccess(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval);
JSBool getVar(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval);

static JSFunctionSpec adm_functions[] = {
  /*    name          native          nargs    */
  {"displayError",      displayError,         1},
  {"displayInfo",       displayInfo,        1},
  {"fileReadSelect",    fileReadSelect,        0},
  {"fileWriteSelect",   fileWriteSelect,        0},
  {"print",             print,        0},
  {"allFilesFrom",      allFilesFrom,        0},
  {"nextFile",          nextFile,        0},
  {"setSuccess",          setSuccess,        1},
  {"getVar",          getVar,        1},

  {0}
};

uint8_t JS_AvidemuxFunction(JSContext *cx,JSObject *global)
{
        if( JS_DefineFunctions(cx, global, adm_functions)==true) return 1;
        
        printf("Error in JSAvidemuxfunction\n");
        return 0;
}
extern char *script_getVar(char *in, int *r);
JSBool getVar(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        int out=0;
        char *dupe=NULL;

       // ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        // default return value
        if(argc != 1)
                return JS_FALSE;
        char  *stringa = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        dupe=script_getVar(stringa ,&out);
        
        if(!dupe)
                return JS_FALSE;
        // if out=1 it is a string else a number
        if(out)
                *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx,dupe));
        else
                *rval = INT_TO_JSVAL(atoi(dupe));
        return JS_TRUE;
}// end AddSegment

JSBool setSuccess(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
//ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
int Jscript_succeed=0;
        // default return value
        if(argc != 1)
                return JS_FALSE;
        
        Jscript_succeed=JSVAL_TO_BOOLEAN(argv[0]);
        JS_setSuccess(Jscript_succeed);
        
        return JS_TRUE;
}// end AddSegment

JSBool displayError(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
  //      ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
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
    //    ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
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
    //    ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        char *name;
        // default return value
        if(argc != 0)
                return JS_FALSE;
        if(!name) return JS_FALSE;
        GUI_FileSelRead(_("Open file (Read mode)"), &name);
        *rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx,name));
        ADM_dealloc(name);
        return JS_TRUE;
}// end AddSegment
JSBool fileWriteSelect(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
   //     ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        char *name;
        // default return value
        if(argc != 0)
                return JS_FALSE;
        GUI_FileSelWrite(_("Open file (Write mode)"), &name);
        if(!name) return JS_FALSE;
        *rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx,name));
        ADM_dealloc(name);
        return JS_TRUE;
}// end AddSegment
JSBool print(JSContext *cx, JSObject *obj, uintN argc, 
                                       jsval *argv, jsval *rval)
{// begin AddSegment
        char *str;
  //      ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        // default return value
        if(argc != 1)
                return JS_FALSE;
        str=JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        fprintf(stderr,"JSConsole:%s\n",str);
        return JS_TRUE;
}// end AddSegment
/*****************************************************
        To process a whole directiry at a time
*******************************************************/
#define ADM_MAX_DIR 1024
static char *dirs[ADM_MAX_DIR];
static int dirmax=0;
static int curdir=0;
static void cleanup( void );
void cleanup( void )
{
        for(int i=0;i<dirmax;i++)
                ADM_dealloc(dirs[i]);
        dirmax=0;
        curdir=0;
}
JSBool allFilesFrom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
char *str;
DIR *dir;
struct dirent *direntry;
       // ADM_JSAvidemux *p = (ADM_JSAvidemux *)JS_GetPrivate(cx, obj);
        cleanup();
        // default return value
        if(argc != 1)
                return JS_FALSE;
        str=JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        dir=opendir(str);
        if(!dir)
        {
                *rval=INT_TO_JSVAL(0); // No files
                return JS_TRUE;
        }
        while((direntry=readdir(dir)) && dirmax<ADM_MAX_DIR-1)
        {
                dirs[dirmax]=(char *)ADM_alloc(strlen(str)+strlen(direntry->d_name)+2);
                strcpy(dirs[dirmax],str);
                strcat(dirs[dirmax],direntry->d_name);
                //printf("File:<%s>\n",dirs[dirmax]);
                dirmax++;
        }
        closedir(dir);
        *rval=INT_TO_JSVAL(dirmax);
        return JS_TRUE;
}

JSBool nextFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
char *n;
        if(argc != 0)
                return JS_FALSE;

        if(curdir==dirmax)
        {
                n="";
        }
        else
        {
                n=dirs[curdir++];
        }
        *rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx,n));
        return JS_TRUE;
}
