#ifndef __OGHEADER__
#define __OGHEADER__
#include "avifmt.h"
#include "avifmt2.h"
#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_oghead.h"
#include "ADM_ogmpages.h"

typedef struct OgIndex
{
	uint32_t flags;
	uint64_t pos;
	uint32_t size;
	uint64_t audioSeen[2];	// Nb of sample seen for track 1/2 
				// the sample are of the equivalent of PCM 16 bytes, so the number gets high
				// -very- quickly. The good news is that once we get the frequency, it is
				// very accurate seeking.
	uint32_t audioData[2];	// accumulative data seen for track1/2 in usefull payload
				// handy for seeking
}OgIndex;
#define NO_FRAG 0xFFFF
class oggAudio :  public AVDMGenericAudioStream
{

		protected:
			    OGMDemuxer			*_demuxer;
			    OgIndex			*_index;
			    uint8_t			_track;
			    uint8_t			_trackIndex;
			    uint32_t			_nbIndex;  
			    uint8_t			_buffer[64*1025];
			    uint32_t			_inBuffer; 
			    uint64_t			_pos;
			    uint8_t 			fillBuffer( void );
			    uint32_t			_lastFrag;
			    uint32_t 			_extraLen;
			    uint8_t  			*_extraData;
			    
 
				
		public:
					oggAudio( char *name,uint32_t nbsync,
						OgIndex *idx,uint8_t trk,uint8_t trkidx );
			virtual 	~oggAudio() ;
			virtual uint8_t goTo(uint32_t offset);
			virtual uint32_t read(uint32_t size,uint8_t *ptr);
			virtual uint32_t readPacket(uint32_t *size, uint8_t *data,uint32_t *flags);
			virtual	uint8_t	 extraData(uint32_t *l,uint8_t **d)
									{
										*l=_extraLen;
										*d=_extraData;
										return 1;
									}		
			virtual uint32_t readDecompress( uint32_t size,uint8_t *ptr );
			virtual uint8_t	 goToTime(uint32_t mstime);

};



class oggHeader         :public vidHeader
{
protected:
             			FILE 			*_fd;
				uint64_t 			_filesize;
				uint32_t			_videoTrack;
				uint32_t			_audioTrack2;
				uint32_t			_audioTrack;
				void				_dump(void);
				OGMDemuxer 			*_demux;				
				uint8_t 			buildIndex(uint32_t  *nb,uint32_t *nbaudio);
				OgIndex				*_index;				
				uint32_t			_lastImage;
				uint32_t			_lastFrag;
				uint32_t			_nbAudioPacket;
				oggAudio			*_audio;

public:

virtual   void 			Dump(void) ;
virtual   uint32_t 		getNbStream(void) ;
virtual   uint8_t 		needDecompress(void) ;

					oggHeader( void ) ;
		   virtual  		~oggHeader(  ) ;
// AVI io
virtual 	uint8_t			open(char *name);
virtual 	uint8_t			close(void) ;
  //__________________________
  //				 Info
  //__________________________

  //__________________________
  //				 Audio
  //__________________________

virtual 	WAVHeader 		*getAudioInfo(void )  ;
virtual 	uint8_t			getAudioStream(AVDMGenericAudioStream **audio);

// Frames
  //__________________________
  //				 video
  //__________________________
virtual 	uint8_t  getFrameSize(uint32_t frame,uint32_t *size) ;

virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags);
virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags) ;
virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
					uint32_t *flags);
virtual 	uint8_t  	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)	;
	     	 		
};




#endif


