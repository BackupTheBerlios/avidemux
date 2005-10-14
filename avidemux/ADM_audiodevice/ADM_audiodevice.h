//
// C++ Interface: ADM_audiodevice
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

typedef enum
{
	DEVICE_OSS=1,
	DEVICE_DUMMY,
	DEVICE_ARTS,
	DEVICE_ALSA,
	DEVICE_COREAUDIO,
	DEVICE_SDL,
	DEVICE_WIN32,
    DEVICE_ESD
}AUDIO_DEVICE;

typedef struct DEVICELIST
{
	AUDIO_DEVICE id;
	char	     *name;
};
#define MKADID(x) {DEVICE_##x,#x}
static const DEVICELIST audioDeviceList[]=
{
	
#ifdef OSS_SUPPORT	
	MKADID(OSS),
#endif	
#ifdef USE_ARTS
	MKADID(ARTS),
#endif	
#ifdef ALSA_SUPPORT
	MKADID(ALSA),
#endif	
#ifdef CONFIG_DARWIN
	MKADID(COREAUDIO),
#endif
#if	defined(USE_SDL) && !defined(CYG_MANGLING)	
	MKADID(SDL),
#endif	
#ifdef CYG_MANGLING	
	MKADID(WIN32),
#endif	
#ifdef USE_ESD	
	MKADID(ESD),
#endif	

	MKADID(DUMMY)
};
