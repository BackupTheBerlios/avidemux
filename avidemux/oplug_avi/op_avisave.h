/***************************************************************************
                          op_avisave.h  -  description
                             -------------------
    begin                : Fri May 3 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef __AVI_SAVE__
 #define   __AVI_SAVE__

 #include "ADM_library/default.h"
 #include "ADM_dialog/DIA_working.h"
 #include "ADM_dialog/DIA_encoding.h"
 

 class GenericAviSave
 {
     protected :
     		DIA_encoding		*encoding_gui;
      	 	AVDMGenericVideoStream  	*_incoming;
     		DIA_working	*dialog_work;
     		uint8_t 		has_audio_track;
           	uint8_t		stored_audio_frame;
           	uint8_t 		has_audio_vbr;
               	uint32_t		one_audio_frame_full;
                uint32_t		one_audio_frame_left;
                uint32_t 		one_pcm_audio_frame;
                int32_t  		one_audio_stock;
               	uint32_t  		fps1000;
                uint32_t 		frametogo;
                char			_name[500];
		uint32_t		_videoProcess;
                uint8_t		_part;
                AVDMGenericAudioStream	*audio_filter;
                AVDMGenericAudioStream	*audio_filter2;

                uint8_t 		computeAudioCBR (void)   ;
                aviWrite		*writter;
                uint8_t		*vbuffer;
                uint8_t		*abuffer;
                uint32_t		_videoFlag;
		uint32_t  	 	_lastIPFrameSent;

               			virtual uint8_t setupAudio( void);
		                virtual uint8_t setupVideo( char *name )=0;
                    virtual uint8_t writeAudioChunk(void );
                    virtual uint8_t writeVideoChunk(uint32_t frame )=0;
                    virtual void guiStart(void);
                    virtual void guiStop(void);
                    virtual void guiSetPhasis(char *str) {UNUSED_ARG(str);};
                    virtual uint8_t guiUpdate(uint32_t nb,uint32_t total);		    
                    uint8_t	handleMuxSize(void ) ; // auto chop
                    uint8_t reigniteChunk( uint32_t len, uint8_t *data );
		    uint32_t searchForward(uint32_t startframe);

						//
						AVIStreamHeader 	_videostreamheader;
						BITMAPINFOHEADER  _bih;
						MainAVIHeader			_mainaviheader;
     public:
     						GenericAviSave( void );
			           	virtual ~GenericAviSave( void );
           				virtual uint8_t saveAvi( char *name );
   };
extern uint32_t muxSize;

  #endif

