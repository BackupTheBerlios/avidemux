/***************************************************************************
                          ADM_mpegscan.h  -  description
                             -------------------
    begin                : Sat Oct 12 2002
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
#ifndef ADM_mpeg2dec
#define ADM_mpeg2dec
 
#define MP2D_BUFFER_SIZE 1024*1024
// MG cache is now the maximum of B-frame in the file
#define MG_CACHE 8

#define ES_ONLY
#include "ADM_mpeg2dec/ADM_mpegpacket.h"

typedef struct
{
	uint8_t type;
	uint64_t offset;			// offset / stream
	uint64_t absoffset;  // offset / file
	uint32_t size;		
}MFrame;
typedef struct 
{
	uint32_t index,real;

}MRenumber;

 class mpeg2decHeader : public vidHeader
{
protected:
	uint8_t			asyncJump(uint32_t frame)	;
public:

				mpeg2decHeader(void);
				~mpeg2decHeader();
//  static int checkFourCC(uint8_t *in, uint8_t *fourcc);

	virtual   void 				Dump(void);
	virtual   uint32_t 			getNbStream(void);
	virtual   uint8_t 			needDecompress(void);

// AVI io
	virtual 	uint8_t			open(char *name);
	virtual 	uint8_t			close(void);
  //__________________________
  //				 Info
  //__________________________

  //__________________________
  //				 Audio
  //__________________________

	virtual 	WAVHeader *getAudioInfo(void )  ;
	virtual 	uint8_t			getAudioStream(AVDMGenericAudioStream **audio);

// Frames
  //__________________________
  //				 video
  //__________________________
		virtual	uint8_t  getExtraHeaderData(uint32_t *len, uint8_t **data);
		virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags);
		virtual 	uint8_t  getFrameSize(uint32_t frame,uint32_t *size);
		virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags);
		virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags);
		virtual 	uint8_t  	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)	;



private:
					uint8_t _video_track_id;
					uint8_t *_end;
					uint8_t _buffer[MP2D_BUFFER_SIZE];


			    // Deals with frame

        virtual 	uint8_t  			getRaw(uint32_t framenum,uint8_t *ptr,uint32_t* framelen);
	virtual 	uint8_t   			getRawStart(uint8_t *ptr,uint32_t *len);


		      	uint32_t					_nbFrames;
			uint32_t 					_nbIFrame;

		        uint8_t					renumber(uint32_t nb);

		      class AVDMMpeg2decAudioStream *_audioStream;

		       ADM_mpegDemuxer		*demuxer;   		     
		      MFrame					*_indexMpegPTS;

		      	uint8_t					*_startSeq;
			uint32_t					_startSeqLen;

public:
			uint8_t					reorder( void ) {return 1;}
			uint8_t					isReordered( void ) { return 1;}

};
#endif
