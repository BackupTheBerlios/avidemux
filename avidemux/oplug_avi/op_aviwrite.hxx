/***************************************************************************
                          op_aviwrite.hxx  -  description
                             -------------------
    begin                : Sun Feb 17 2002
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

 #ifndef __op_avi__
 #define __op_avi__
 class  aviWrite
 {
 protected:
		FILE 		*_out;
                ADMFile         *_file;
                
		MainAVIHeader	_mainheader;
		AVIStreamHeader _videostream;
		BITMAPINFOHEADER _bih;
		AVIStreamHeader _audio1,_audio2;
		uint32_t nb_audio;
		uint32_t asize;
		uint32_t asize2;

		uint32_t vframe;

		uint32_t curindex;
		AviList *LAll ;
		AviList	*LMovie ;
		AviList *LMain;

    uint8_t saveFrame(uint32_t len,uint32_t flags,
													uint8_t *data,uint8_t *fcc)			;			

		
		uint8_t writeMainHeader( void );
		uint8_t writeVideoHeader( uint8_t *extra, uint32_t extraLen );
		uint8_t writeAudioHeader (	AVDMGenericAudioStream * stream, AVIStreamHeader *header );

	uint8_t setStreamInfo(ADMFile *fo,
							uint8_t 	*stream,
                   					uint8_t 	*info,
							uint32_t 	infolen,
			uint8_t		*extra,
			uint32_t 	extraLen,
							uint32_t  	maxxed);
	uint8_t updateHeader(MainAVIHeader *mainheader,
									AVIStreamHeader *videostream,
									AVIStreamHeader *astream);
 public:
		aviWrite(void);	
  		uint8_t saveBegin(char *name,
							MainAVIHeader *mainheader,
							uint32_t nb_frame,
			AVIStreamHeader 	*invideostream,
			BITMAPINFOHEADER	*bitmap,	
			uint8_t 		*videoextra,
			uint32_t		videoextraLen,
			AVDMGenericAudioStream 	*audiostream,
							AVDMGenericAudioStream 	*audiostream2=NULL);

      uint8_t setEnd(void);

	uint8_t saveVideoFrame(uint32_t len,uint32_t flags,uint8_t *data);
	uint8_t saveAudioFrame(uint32_t len,uint8_t *data) ;
	uint8_t saveAudioFrameDual(uint32_t len,uint8_t *data) ;
	uint32_t	getPos( void );
	uint8_t 	sync( void );


 };
 #endif
                        	
