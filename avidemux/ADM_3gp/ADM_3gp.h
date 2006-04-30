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
	uint32_t intra;
	uint64_t time;

}_3gpIndex;
class _3gpTrack
{
public:
    _3gpIndex   *index;
    uint32_t    id;
    uint32_t    nbIndex;
    uint32_t    extraDataSize;
    uint8_t     *extraData;
    WAVHeader   _rdWav;
                _3gpTrack(void);
                ~_3gpTrack();
};

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
		uint32_t					_extraLen;
		uint8_t						*_extraData;
		uint32_t                                         _audioDuration;
		
		
public:
					_3gpAudio(FILE *fd,_3gpTrack *trak);
// _3gpIndex *idx,
// 						uint32_t nbchunk, FILE * fd,WAVHeader *incoming,
// 						uint32_t extraLen,uint8_t *extraData,uint32_t duration);
	virtual				~_3gpAudio();
        virtual uint32_t 		read(uint32_t len,uint8_t *buffer);
        virtual uint8_t  		goTo(uint32_t newoffset);
		   uint8_t			getNbChunk(uint32_t *ch);
	virtual uint8_t 		getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
	virtual uint8_t 		goToTime(uint32_t mstime);
	virtual uint8_t			extraData(uint32_t *l,uint8_t **d);

};

#define _3GP_MAX_TRACKS 8
#define VDEO _tracks[0]
#define ADIO _tracks[nbAudioTrack+1]._rdWav
class _3GPHeader         :public vidHeader
{
protected:
          uint8_t                       _reordered;		
	  FILE 				*_fd;
          _3gpTrack                     _tracks[_3GP_MAX_TRACKS];
	  uint32_t                      _audioDuration;
          uint32_t                      _currentAudioTrack;
	uint8_t 			parseAtomTree(adm_atom *atom);
	  _3gpAudio			*_audioTracks[_3GP_MAX_TRACKS-1];
	uint8_t 			sync(_3gpIndex *idx,uint32_t index_size, uint32_t sync_size,uint32_t *sync);
         uint32_t  nbAudioTrack;
	 uint32_t *Sz,*Co,*Sc;
	 uint32_t *Sn,*Sync;
	 uint32_t *SttsN,*SttsC,*Ctts,nbCtts;


	uint8_t		buildIndex(	_3gpTrack *track,
					uint32_t scale,
					uint32_t nbSz,		uint32_t *Sz,
					uint32_t nbChunk ,	uint32_t *Chunk,
					uint32_t nbSc,		uint32_t *Sc,
					uint32_t nbStts,uint32_t *SttsN,uint32_t *SttsC,
					uint32_t *Sn,			uint32_t *outNbChunk
					);
	uint32_t 		readPackedLen(adm_atom *tom );
	
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
virtual   uint8_t                       getExtraHeaderData(uint32_t *len, uint8_t **data);
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
// Multi track
uint8_t        changeAudioStream(uint32_t newstream);
uint32_t     getCurrentAudioStreamNumber(void);
uint8_t     getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **infos);
uint8_t      isReordered( void );
uint8_t      reorder( void );

};

uint8_t extractMpeg4Info(uint8_t *data,uint32_t dataSize,uint32_t *w,uint32_t *h);
uint8_t extractH263Info(uint8_t *data,uint32_t dataSize,uint32_t *w,uint32_t *h);
#endif


