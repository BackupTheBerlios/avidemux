#ifndef _ADM_JSAVIDEMUXAUDIO_H
#define _ADM_JSAVIDEMUXAUDIO_H

#pragma once

// Spidermonkey
#include <jsapi.h>
#include "ADM_AvidemuxAudio.h"
#include <config.h>

class ADM_JSAvidemuxAudio
{
public:
	ADM_JSAvidemuxAudio(void) : m_pObject(NULL) {}
	virtual ~ADM_JSAvidemuxAudio(void);

	static JSBool JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool JSConstructor(JSContext *cx, JSObject *obj, uintN argc, 
								jsval *argv, jsval *rval);
	static void JSDestructor(JSContext *cx, JSObject *obj);
	static JSObject *JSInit(JSContext *cx, JSObject *obj, JSObject *proto = NULL);
	static JSBool ScanVBR(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	static JSBool Save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	static JSBool Load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	static JSBool Reset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	static JSBool Codec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool getNbTracks(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool setTrack(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool lamePreset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	static JSPropertySpec avidemuxaudio_properties[];
	static JSFunctionSpec avidemuxaudio_methods[];
	enum
	{
                audioprocess_prop,
		normalize_prop,
		downsample_prop,
		resample_prop,
		delay_prop,
		film2pal_prop,
		pal2film_prop,
		mono2stereo_prop,
		stereo2mono_prop
	};
	static JSClass m_classAvidemuxAudio;
        


//protected:
	void setObject(ADM_AvidemuxAudio *pObject);
	ADM_AvidemuxAudio *getObject();

private:
	ADM_AvidemuxAudio *m_pObject;

};

#endif
