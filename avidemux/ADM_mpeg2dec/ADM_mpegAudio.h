/***************************************************************************
                          ADM_mpegAudio.h  -  description
                             -------------------
    begin                : Sun Oct 27 2002
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
 #ifndef MPEG2DEC_AUDIO
 #define MPEG2DEC_AUDIO
 
 typedef struct SYNC_
 {
		uint64_t relative;	 
		uint64_t absolute;	 	 
};
 
  class AVDMMpeg2decAudioStream : public AVDMGenericAudioStream
{
   	protected:
 
       protected:
					    ADM_mpegDemuxer		*demuxer;	
					    uint32_t								_syncPoints;
					    SYNC_								*_sync;
					    uint8_t 								getAudioSpec(void);
				     	uint8_t 								getAC3AudioSpec(void);
				
		public:
					AVDMMpeg2decAudioStream( char *name,uint32_t nbsync );
					virtual 						~AVDMMpeg2decAudioStream() ;          		
					virtual uint8_t 			goTo(uint32_t offset);
					virtual uint32_t 			read(uint32_t size,uint8_t *ptr);

}
;
#endif
