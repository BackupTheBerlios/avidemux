/***************************************************************************
                          audioprocess.hxx  -  description
                             -------------------
    begin                : Sun Jan 13 2002
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

#ifndef __AudioProcess__
#define __AudioProcess__

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioeng_sox.h"
#define MINUS_ONE 0xffffffff
#define PROCESS_BUFFER_SIZE 48000*4*4 // should be enougth
#define MINIMUM_BUFFER   8192*4
//_______________________________________
//_______________________________________
class AVDMProcessAudioStream : public AVDMGenericAudioStream
{
protected:
       AVDMGenericAudioStream *_instream;


public:

        AVDMProcessAudioStream(AVDMGenericAudioStream *instream);
	virtual uint8_t  preprocess( void );
	virtual uint8_t  configure( void );
//

        virtual uint32_t read(uint32_t len,uint8_t *buffer)=0;
        virtual uint8_t  goTo(uint32_t newoffset);
        virtual uint8_t  goToTime(uint32_t newoffset);
	virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer)=0;


};
class AVDMBufferedAudioStream : public  AVDMProcessAudioStream
{
protected:

			 int16_t   _buffer[PROCESS_BUFFER_SIZE];
			// uint32_t  	_bufferlen;

			 virtual uint32_t grab(uint8_t *outbuffer)=0;
			 uint32_t	_headBuff,_tailBuff;
public:

        AVDMBufferedAudioStream(AVDMGenericAudioStream *instream);
				virtual uint8_t  preprocess( void ) {return 1;};
				virtual uint8_t  configure( void )  { return 1;};
//

        virtual uint32_t read(uint32_t len,uint8_t *buffer);
        virtual uint8_t  goTo(uint32_t newoffset);
        virtual uint8_t  goToTime(uint32_t newoffset);
	virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer)
						{
             		return read(len,buffer);
						};


};
//_____________________________________________
class AVDMProcessAudio_Normalize : public AVDMProcessAudioStream
{
protected:
         int16_t   _table[32768*2+1];
				 uint32_t								_max;
				 uint32_t								_scanned;
public:

        AVDMProcessAudio_Normalize(AVDMGenericAudioStream *instream);
        ~AVDMProcessAudio_Normalize();
				virtual uint8_t  preprocess( void ) ;

        virtual uint32_t read(uint32_t len,uint8_t *buffer);
			  virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer);
        virtual uint8_t  goToTime(uint32_t newoffset);

};

//_______________________________________________________________
typedef struct DRCparam
{
    uint32_t mUseGain;
    double   mFloor;
    double   mAttackTime;
    double   mDecayTime;
    double   mRatio;
    double   mThresholdDB;
   // double   mGainDB;  
};
class AVDMProcessAudio_Compress : public AVDMBufferedAudioStream
{
protected:
#define DRC_WINDOW 100
				 uint8_t            filled;
				 DRCparam           _param;			
				 virtual uint32_t 	grab(uint8_t *obuffer);
				 
				 double             mCircle[DRC_WINDOW];
				 double             mLevelCircle[DRC_WINDOW];	
				 int                mCircleSize;
				 
				 int                mCirclePos;	
				 double             mRMSSum;
				 double             mThreshold;
				 double             mGain;
				 double             mAttackFactor;
				 double             mDecayFactor;	
				 double             mLastLevel;
				 double             mGainDB;
				 
				 double             AvgCircle(double value);
				 void               Follow(double x, double *outEnv, int maxBack);
				 float              DoCompression(float value, double env);
				 void               drc_cleanup(void);
	#define ONE_CHUNK 1000			 
				 double             follow[ONE_CHUNK/2];
				 double             value[ONE_CHUNK/2];
				 	
public:
						
        AVDMProcessAudio_Compress(AVDMGenericAudioStream *instream,DRCparam *p);
		 ~AVDMProcessAudio_Compress();

     		
};
//_____________________________________________
class AVDMProcessMono2Stereo : public AVDMBufferedAudioStream
{
protected:
                virtual uint32_t 	grab(uint8_t *obuffer);	
			uint8_t		_engaged;
public:
						
        AVDMProcessMono2Stereo(AVDMGenericAudioStream *instream);
        ~AVDMProcessMono2Stereo();
     		
};
class AVDMProcessStereo2Mono : public AVDMBufferedAudioStream
{
protected:
                virtual uint32_t 	grab(uint8_t *obuffer);	
			uint8_t		_engaged;
public:
						
        AVDMProcessStereo2Mono(AVDMGenericAudioStream *instream);
        ~AVDMProcessStereo2Mono();
     		
};

//_____________________________________________
class AVDMProcessAudio_Null : public AVDMBufferedAudioStream
{
protected:
				 uint32_t _served;
         			uint32_t _start_time;
         			uint32_t _size;
        
	
public:
						
				~AVDMProcessAudio_Null();
        			AVDMProcessAudio_Null(AVDMGenericAudioStream *instream
						,uint32_t time_offset, uint32_t duration);
       		virtual uint32_t 	grab(uint8_t *obuffer);
		virtual uint8_t  goToTime(uint32_t newoffset);
     		
};
//_______________________________________________________________
class AVDMProcessAudio_Resample : public AVDMBufferedAudioStream
{
protected:

				 uint8_t 		_quality;

 			  virtual uint32_t 	grab(uint8_t *obuffer);
				void 							InitialSRC( void );
				uint32_t  				DownKernel(uint8_t *out);

public:
						
        AVDMProcessAudio_Resample(AVDMGenericAudioStream *instream,uint8_t qual);
			  ~AVDMProcessAudio_Resample() { delete _wavheader;};    	     		
};
//_______________________________________________________________
class AVDMProcessAudio_SoxResample : public AVDMBufferedAudioStream
{
protected:

				
	  uint32_t		_frequency;
	  ResampleStruct	_resample;
	  ResampleStruct	_resample2;
	  uint8_t		_pass;
	  uint32_t		_head,_tail;
	  virtual uint32_t 	grab(uint8_t *obuffer);
	  

public:
						
          AVDMProcessAudio_SoxResample(AVDMGenericAudioStream *instream,uint32_t  frequency);
	  ~AVDMProcessAudio_SoxResample();  
	   virtual uint8_t  goToTime(uint32_t newoffset);	     		
};
//_____________________________________________
class AVDMProcessAudio_RawShift : public AVDMProcessAudioStream
{
protected:


			
        		 uint32_t  _starttime;		// actual start time
			 int32_t   _msoff;		// copy of parameter
			 uint8_t   _filterOn;		// filter active ?
     			 uint32_t  _startoffset;
			 uint32_t  _sampleOffset;


		
public:
						
				~AVDMProcessAudio_RawShift();
        			AVDMProcessAudio_RawShift(AVDMGenericAudioStream *instream,
					int32_t msoffset,
					uint32_t starttime	);		 // offset in ms
        	virtual uint32_t read(uint32_t len,uint8_t *buffer);
		virtual uint8_t  goTo(uint32_t newoffset);
		virtual uint8_t  goToTime(uint32_t newoffset);
		virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer);
  
		virtual uint8_t		isVBR(void ) { return _instream->isVBR();};
		virtual uint8_t		extraData(uint32_t *l,uint8_t **d)
					{
						 return _instream->extraData(l,d);
					}
		virtual	uint8_t		getPacket(uint8_t *dest, uint32_t *len, 
							uint32_t *samples);
		   		
};
 //_____________________________________________
class AVDMProcessAudio_TimeShift : public AVDMProcessAudioStream
{
protected:


			
         int32_t  _timeoffset;
				 int32_t  _byteoffset;				
				 int32_t  _outpos;

		
public:
						
				~AVDMProcessAudio_TimeShift();
				/*uint8_t	initLame(uint32_t frequence,
												 uint32_t mode,
												 uint32_t bitrate);*/

        AVDMProcessAudio_TimeShift(AVDMGenericAudioStream *instream,
															int32_t msoffset	);		 // offset in ms
        virtual uint32_t read(uint32_t len,uint8_t *buffer);
				virtual uint8_t  goToTime(uint32_t newoffset);
			  virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer);
     		
};
//_____________________________________________
class AVDMProcessAudio_LEBE : public AVDMProcessAudioStream
{
protected:
		
public:
						
				~AVDMProcessAudio_LEBE();

        		AVDMProcessAudio_LEBE(AVDMGenericAudioStream *instream 	);		 
        virtual uint32_t read(uint32_t len,uint8_t *buffer);
	virtual uint8_t  goTo(uint32_t newoffset);
    	virtual uint8_t  goToTime(uint32_t newoffset);
  	virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer);
     		
};
#include "ADM_audiofilter/audioeng_lame.h"
#endif
// EOF
