/***************************************************************************
                          ADM_pics.h  -  description
                             -------------------
    begin                : Mon Jun 3 2002
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
 


#ifndef __3GPHEADER__
#define __3GPHEADER__
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_3gp/ADM_atom.h"

typedef struct _3gpIndex
{
	uint64_t offset;
	uint64_t size;
	uint8_t  intra;

}_3gpIndex;
//
//	Audio track
//
class _3gpAudio : public AVDMGenericAudioStream
{
protected:

           	uint32_t 					_nb_chunks;
		uint64_t					_abs_position;
		uint32_t					_rel_position;

              	uint32_t 					_current_index;
	    	_3gpIndex 					*_index;
		FILE						*_fd;

		
		
		
public:
					_3gpAudio(_3gpIndex *idx,
						uint32_t nbchunk, FILE * fd,WAVHeader *incoming);
	virtual				~_3gpAudio();
        virtual uint32_t 		read(uint32_t len,uint8_t *buffer);
        virtual uint8_t  		goTo(uint32_t newoffset);
		   uint8_t			getNbChunk(uint32_t *ch);
	virtual uint8_t 		getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);

};



class _3GPHeader         :public vidHeader
{
protected:
       				
	  FILE 				*_fd;
	  _3gpIndex 			*_idx;
	  _3gpIndex 			*_audioIdx;
	  uint32_t				_nbAudioChunk;
	  uint32_t				_volHeader;
	  uint32_t				_volHeaderLen;
	uint8_t 				parseAtomTree(adm_atom *atom);
	  _3gpAudio			*_audioTrack;
	uint8_t 				sync(_3gpIndex *idx,uint32_t index_size, uint32_t sync_size,uint32_t *sync);

	 uint32_t *Sz,*Co,*Sc;
	 uint32_t *Sn,*Sync;


	uint8_t		buildIndex(	_3gpIndex **idx,
					uint32_t nbSz,		uint32_t *Sz,
					uint32_t nbChunk ,	uint32_t *Chunk,
					uint32_t nbSc,		uint32_t *Sc,
					uint32_t *Sn,			uint32_t *outNbChunk);
	uint32_t 		readPackedLen(adm_atom *tom );
	WAVHeader		*_rdWav;
public:

virtual   void 				Dump(void) {};
virtual   uint32_t 			getNbStream(void) ;
virtual   uint8_t 			needDecompress(void) { return 1;};

			_3GPHeader( void ) ;
       		    	~_3GPHeader(  ) ;
// AVI io
virtual 	uint8_t			open(char *name);
virtual 	uint8_t			close(void) ;
  //__________________________
  //				 Info
  //__________________________

  //__________________________
  //				 Audio
  //__________________________

virtual 	WAVHeader 	*getAudioInfo(void ); 
virtual 	uint8_t		getAudioStream(AVDMGenericAudioStream **audio);

// Frames
  //__________________________
  //				 video
  //__________________________

virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags);
virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags);
virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags);
virtual 	uint8_t  	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)	;
 virtual     uint8_t getFrameSize (uint32_t frame, uint32_t * size);
	     	 		
};

#endif


