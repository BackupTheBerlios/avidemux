/***************************************************************************
                          ADM_edit.hxx  -  description
                             -------------------
    This file is the composer
    It presents the processed underlying files as if it was a flat
    file.
    Very useful for cut/copy/merge etc...


    The frame seen by GUI/user is converted in seg (segment number)
    	and segrel, the frame number compared to the beginning of the movie
    	described by the segment
    	** NOT the beginning of the segment start_frame **



    begin                : Thu Feb 28 2002
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
 #ifndef __ADM_composer__
 #define __ADM_composer__
 #include "ADM_editor/ADM_Video.h"
 #include "ADM_codecs/ADM_codec.h"
#define MAX_SEG  	100 // Should be enougth
#define MAX_VIDEO   100
typedef enum
{
		Unknown_FileType=0,
		AVI_FileType=1,
    		Mpeg_FileType=2,
		Nuppel_FileType=3    ,
		BMP_FileType=4  ,
		MpegIdx_FileType=5,
		H263_FileType=6,
		Mp4_FileType=7,
		WorkBench_FileType=8,
		Ogg_FileType=9,
		_3GPP_FileType=10,
		VCodec_FileType=11,
		DUMMY_FILETYPE=99
}fileType;


//
//  The start frame correspond to the frame 0 of the segment (quite obvisous)
//  _nb_video_frames is the number of active frame in the segment
//
//
typedef struct
{
  	vidHeader 							*_aviheader;
  	decoders							*decoder;

	uint32_t  								_audio_size;
	uint32_t								_audio_duration;
	AVDMGenericAudioStream 			*_audiostream;
	uint8_t								_isAudioVbr;
	WAVHeader							*_wavheader;

	uint32_t								_nb_video_frames;
	uint32_t								_forwardReference;
	uint8_t								*_forwardFrame;
	uint32_t								_forwardFlag;
	uint8_t								_reorderReady;
}_VIDEOS;


typedef struct
{
  	uint32_t							_reference;
 	uint32_t							_start_frame;
	uint32_t							_nb_frames;
	uint32_t  							_audio_size;
	uint32_t							_seg_audio_duration;
	uint32_t							_seg_video_duration;
 	uint32_t  							_audio_start;
}_SEGMENT;

class ADM_Composer
{
  private:
  					uint8_t 	setForward(uint32_t ref, uint32_t frame);
  					uint32_t 	_nb_segment;
					uint32_t 	_nb_video;
					uint32_t  _nb_clipboard;
  					uint32_t 	_total_frames;
  					uint32_t 	_audio_size;
  					// _audiooffset points to the offset / the total segment
  					// not the used part !
  					uint32_t  _audioseg;
  					uint32_t  _audiooffset;
       					uint8_t    _cached;
					uint8_t	   _haveMarkers; // used for load/save edl

       					uint32_t _lastseg,_lastframe,_lastlen;

  					_SEGMENT 		_segments[MAX_SEG];
					_SEGMENT 		_clipboard[MAX_SEG];
					_VIDEOS 		_videos[MAX_VIDEO];

						uint8_t  	convFrame2Seg(uint32_t framenum,uint32_t *seg,
																			uint32_t *relframe);
						uint8_t  	convSeg2Frame(	uint32_t *framenum,
																			uint32_t seg,
																			uint32_t relframe);
						uint8_t 	crunch( void)																			;
						uint8_t 	duplicateSegment( uint32_t segno);
						uint32_t 	computeTotalFrames(void) ;

						uint8_t 	removeTo( 	uint32_t to, uint32_t seg,uint8_t included);
						uint8_t 	removeFrom( uint32_t from, uint32_t seg,uint8_t included);
						uint8_t 	checkInSeg( uint32_t seg, uint32_t frame);
						uint8_t 	sanityCheck( void);
				       		uint8_t  	updateAudioTrack(uint32_t seg);
					   	void 		deleteAllSegments(void);
						void 		deleteAllVideos(void );

						uint8_t 	getMagic(char *name,uint32_t *magic);
						uint8_t 	identify(char *name, fileType *type);
						uint32_t 	searchForwardSeg(uint32_t startframe);

  public:
  						uint8_t	activateCache( void ){ _cached=1;_lastseg=0xffffff;return 1;}
						uint8_t	flushCache( void ){ _lastseg=0xffffff;return 1;}
						uint8_t	desactivateCache( void ){ _cached=0;return 1;}
  						uint8_t 	getExtraHeaderData(uint32_t *len, uint8_t **data);
  								ADM_Composer();
  				virtual 			~ADM_Composer();
  						void		clean( void );
  						void		dumpSeg(void);
						uint8_t 	saveWorbench(char *name)      ;
						uint8_t 	loadWorbench(char *name)      ;
						uint8_t     resetSeg( void );
						uint8_t	reorder( void );
						uint8_t	isReordered( uint32_t framenum );
						uint8_t	isIndexable( void);
  				//_______________________
  				// specific to composer
  				//_______________________
  						uint8_t 	addFile(char *name,uint8_t mode=0);
  						uint8_t 	cleanup( void);
			   			uint8_t 	isMultiSeg( void) { if(_nb_segment>1) return 1; else return 0;}
  						uint8_t 	removeFrames(uint32_t start,uint32_t end);
  						uint8_t 	addFrameFrom(uint32_t to,uint32_t frombegin,uint32_t fromend);
						uint8_t 	copyToClipBoard (uint32_t start, uint32_t end);
						uint8_t 	pasteFromClipBoard (uint32_t whereto);
  				//_____________________________
  				// navigation & frame functions
  				//_____________________________
  						uint8_t 	getFrame(uint32_t framenum,uint8_t **ptr,
  											uint32_t* 	framelen,uint32_t *flags=0);
						uint8_t  	getRaw(uint32_t framenum,uint8_t *ptr,uint32_t* framelen);
						uint8_t   	getRawStart(uint32_t frame,uint8_t *ptr,uint32_t *len);


   						uint8_t 	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags=0,  uint8_t *seq=0);
	          				uint32_t 	getTime(uint32_t fn);
						uint32_t 	getFlags(uint32_t frame,uint32_t *flags);
						uint32_t 	getFlagsAndSeg (uint32_t frame, 
									uint32_t * flags,uint32_t *segs);
						uint8_t  	setFlag(uint32_t frame,uint32_t flags);
						uint8_t	updateVideoInfo(aviInfo *info);

						uint8_t  	getFrameSize(uint32_t frame,uint32_t *size) ;
						uint8_t		sanityCheckRef(uint32_t start, uint32_t end,
									uint32_t *fatal);
					//______________________________
					//    Info etc... to be removed later
					//______________________________

			     		uint8_t 			setDecodeParam( uint32_t frame );
	 				AVIStreamHeader 	*getVideoStreamHeader(void ) ;
	 				MainAVIHeader 		*getMainHeader(void );
	 				BITMAPINFOHEADER 	*getBIH(void ) ;
	  				uint8_t			getVideoInfo(aviInfo *info);
					WAVHeader 		*getAudioInfo(void )  ;
					uint32_t 			getAudioLength(void);
					uint8_t			getAudioStream(AVDMGenericAudioStream **audio);
					uint8_t			getAudioExtra(uint32_t *l,uint8_t **d);
					uint8_t			audioGoTo(uint32_t offset);
					uint32_t			audioRead(uint32_t len,uint8_t *buffer);
					uint8_t 			audioGoToFn(uint32_t seg,uint32_t fn,uint32_t *noff);
					uint8_t 			audioFnToOff(uint32_t seg,uint32_t fn,uint32_t *noff);
                  //
                  //	Coder/decoder
                  //
                  			uint8_t			getUncompressedFrame(uint32_t frame,uint8_t *out,uint32_t *flagz=NULL)    ;
	     	  			uint8_t			getUncompressedFramePKF(uint32_t *frame,uint8_t *out)    ;
	     	  			uint8_t			getUncompressedFrameNKF(uint32_t *frame,uint8_t *out)    ;

              				uint8_t			searchNextKeyFrame(uint32_t in,uint32_t *oseg, uint32_t * orel);
                 			uint8_t			searchPreviousKeyFrame(uint32_t in,uint32_t *oseg, uint32_t * orel);

					uint8_t   			rebuildFrameType ( void);
                  // kludg
                  			void 				propagateBuildMap( void );
                  			uint32_t			readPCMeq(uint32_t lenasked,
                  										uint8_t *out,uint32_t *lenout);
			virtual 	uint8_t			audioGoToTime(uint32_t mstime,uint32_t *off);
					uint8_t getMarkers(uint32_t *start, uint32_t *end); // get markers from file
										

};
#endif
