#ifndef __AVI_VARS
#define __AVI_VARS
#include "default.h"
#include "fourcc.h"
#include "ADM_editor/ADM_edit.hxx"
//----------------------


#define HAVE_ENCODER


//----------------------
//----------------------
//----------------------
#if (defined( HAVE_LIBESD) && defined(HAVE_ESD_H)) || \
 defined(OSS_SUPPORT) || defined (USE_ARTS) || \
  defined(USE_SDL) || defined(__APPLE__) || \
  defined(__WIN32) || defined(ALSA_SUPPORT)
  
#define HAVE_AUDIO
#endif
//----------------------



#ifdef __DECLARE__
#define EXTERN 
#else
#define EXTERN extern
#endif

//
#define DEBUG
#define DEBUG_L2



EXTERN ADM_Composer *video_body
#ifdef __DECLARE__
=NULL
#endif
;

EXTERN uint32_t  curframe;
EXTERN uint32_t verbose;
EXTERN uint32_t originalPriority;
EXTERN uint8_t 	playing
#ifdef __DECLARE__
=0
#endif
;


EXTERN AVDMGenericAudioStream *aviaudiostream
#ifdef __DECLARE__
=(AVDMGenericAudioStream *)NULL
#endif
;
EXTERN AVDMGenericAudioStream *currentaudiostream
#ifdef __DECLARE__
=(AVDMGenericAudioStream *)NULL
#endif
;
EXTERN AVDMGenericAudioStream *secondaudiostream
#ifdef __DECLARE__
=(AVDMGenericAudioStream *)NULL
#endif
;

;
EXTERN aviInfo   *avifileinfo
#ifdef __DECLARE__
=(aviInfo *)NULL
#endif
;
EXTERN WAVHeader *wavinfo
#ifdef __DECLARE__
=(WAVHeader *)NULL
#endif
;
/**
	If set to 1, means video is in process mode_preview
	If set to 0, copy mode
*/
EXTERN uint32_t audioProcessMode(void);
/**
	If set to 1, means video is in process mode_preview
	If set to 0, copy mode
*/
EXTERN uint32_t videoProcessMode(void);

EXTERN uint32_t frameStart,frameEnd;




#endif
