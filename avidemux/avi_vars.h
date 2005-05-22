#ifndef __AVI_VARS
#define __AVI_VARS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fourcc.h"
#include "config.h"
#include "ADM_editor/ADM_edit.hxx"
//----------------------


#define HAVE_ENCODER


//----------------------
//----------------------
//----------------------
#if (defined( HAVE_LIBESD) && defined(HAVE_ESD_H)) || \
 defined(OSS_SUPPORT) || defined (USE_ARTS) || \
  defined(USE_SDL) || defined(CONFIG_DARWIN) || \
  defined(CYG_MANGLING)
  
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
EXTERN uint8_t 	playing
#ifdef __DECLARE__
=0
#endif
;
EXTERN uint8_t 	mode_preview
#ifdef __DECLARE__
=0
#endif
;

;

/**
	This buffer is share for GUI display
	It leads to the buffer where the last displayed frame has been uncompressed

*/
EXTERN ADMImage *rdr_decomp_buffer
#ifdef __DECLARE__
=NULL
#endif
;



EXTERN AVDMGenericAudioStream *aviaudiostream;
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

EXTERN uint8_t secondaudiostream_isac3
#ifdef __DECLARE__
=0 // else mp3
#endif
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
	Output mode : 
		If 0 display incoming video_body	
		if 1, display filtered video
*/
EXTERN uint8_t guiOutputDisplay
#ifdef __DECLARE__
=0
#endif
;
/**
	If set to 1, means video is in process mode_preview
	If set to 0, copy mode
*/
EXTERN uint32_t audioProcessMode
#ifdef __DECLARE__
=0; // 1-> Full processing mode, 0 copy mode
#endif
;
/**
	If set to 1, means video is in process mode_preview
	If set to 0, copy mode
*/
EXTERN uint32_t videoProcessMode
#ifdef __DECLARE__
=0; // 1-> Full processing mode, 0 copy mode
#endif
;


EXTERN uint32_t frameStart,frameEnd;




#endif
