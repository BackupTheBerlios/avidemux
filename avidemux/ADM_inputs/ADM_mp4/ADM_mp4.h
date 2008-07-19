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
#include "ADM_inputs/ADM_mp4/ADM_atom.h"


class MPsampleinfo
{
  public:
      uint32_t nbCo;
      uint32_t SzIndentical;
      uint32_t nbSz;
      uint32_t nbSc;
      uint32_t nbStts;
      uint32_t nbSync;
      uint32_t nbCtts;
      
	  uint64_t *Co;
      uint32_t *Sz;
      uint32_t *Sc;
      uint32_t *Sn;
      uint32_t *SttsN;
      uint32_t *SttsC;
      uint32_t *Sync; 
      uint32_t *Ctts;
  
      uint32_t samplePerPacket;
      uint32_t bytePerPacket;
      uint32_t bytePerFrame;
      
      MPsampleinfo(void);
      ~MPsampleinfo(void);
};

typedef struct MP4Index
{
	uint64_t offset; // Offset in file to get frame
	uint64_t size;   // Size of frame in bytes
	uint32_t intra;  // Flags associated with frame
	uint64_t time;   // Decoder time in ms
        uint32_t deltaPtsDts; // Delta in frame between pts & dts

}MP4Index;
class MP4Track
{
public:
    MP4Index   *index;
    uint32_t    id;
    uint32_t    scale;
    uint32_t    nbIndex;
    uint32_t    extraDataSize;
    uint8_t     *extraData;
    WAVHeader   _rdWav;
                MP4Track(void);
                ~MP4Track();
};

//
//	Audio track
//
class MP4Audio : public AVDMGenericAudioStream
{
protected:

           	uint32_t 					_nb_chunks;
              	uint32_t 					_current_index;
	    	MP4Index 					*_index;
		FILE						*_fd;
		uint32_t					_extraLen;
		uint8_t						*_extraData;
		uint64_t                                         _audioDuration;
		
		
public:
					MP4Audio(const char *name,MP4Track *trak);
// MP4Index *idx,
// 						uint32_t nbchunk, FILE * fd,WAVHeader *incoming,
// 						uint32_t extraLen,uint8_t *extraData,uint32_t duration);
	virtual				~MP4Audio();
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

class MP4Header         :public vidHeader
{
protected:
          /*****************************/
          uint8_t                       lookupMainAtoms(void *tom);
          void                          parseMvhd(void *tom);
          uint8_t                       parseTrack(void *ztom);
          uint8_t                       decodeVideoAtom(void *ztom);
          uint8_t                       parseMdia(void *ztom,uint32_t *trackType,uint32_t w, uint32_t h);
          uint8_t                       parseStbl(void *ztom,uint32_t trackType,uint32_t w,uint32_t h,uint32_t trackScale);
          uint8_t                       decodeEsds(void *ztom,uint32_t trackType);
          uint8_t                       updateCtts(MPsampleinfo *info );
          uint32_t                      _videoScale;
          int64_t						_movieDuration; // in ms
          uint32_t                      _videoFound;
          uint8_t	                indexify(
                                                MP4Track *track,   
                                                uint32_t trackScale,
                                              MPsampleinfo *info,
                                              uint32_t isAudio,
                                              uint32_t *outNbChunk);
          /*****************************/
        uint8_t                       _reordered;		
        FILE                          *_fd;
        MP4Track                      _tracks[_3GP_MAX_TRACKS];
        int64_t                      _audioDuration;
        uint32_t                      _currentAudioTrack;
        uint8_t                       parseAtomTree(adm_atom *atom);
        MP4Audio                      *_audioTracks[_3GP_MAX_TRACKS-1];
        uint32_t                      nbAudioTrack;
         /*********************************/
	uint32_t                         readPackedLen(adm_atom *tom );
	
public:
          uint8_t               hasPtsDts(void) {return 1;} // Return 1 if the container gives PTS & DTS info
          uint32_t              ptsDtsDelta(uint32_t framenum);
virtual   void 	                Dump(void) {};

                                MP4Header( void ) ;
virtual	                        ~MP4Header(  ) ;
// AVI io
virtual 	uint8_t	       open(const char *name);
virtual 	uint8_t	       close(void) ;
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
virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,ADMCompressedImage *img);
 virtual     uint8_t getFrameSize (uint32_t frame, uint32_t * size);
// Multi track
uint8_t        changeAudioStream(uint32_t newstream);
uint32_t     getCurrentAudioStreamNumber(void);
uint8_t     getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **infos);
uint8_t      isReordered( void );
uint8_t      reorder( void );

};

#endif


