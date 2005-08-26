/***************************************************************************
                          op_mpegwrite.cpp  -  description
                             -------------------
	
	
	Taking >4 as first pass qz is important as there is a huge distortion around
		qz=2 making the final size impredictible

    begin                : Sat Aug 3 2002
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>




#include "ADM_lavcodec.h"
#include "fourcc.h"
#include "ADM_toolkit/ADM_quota.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "subchunk.h"

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encConfig.h"




#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"


//#include "aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_working.h"


//#include "oplug_mpeg/op_mpeg_support.h"
#include "mpeg2enc/ADM_mpeg2enc.h"
#include "oplug_mpeg/op_mpeg_write.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"


#include "ADM_dialog/DIA_enter.h"



#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG2ENC
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_dialog/DIA_encoding.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"



#warning FIXME: Duplicate define with mpeg2enc -> bad
#define MPEG_PREFILL 5

#include "ADM_encoder/adm_encConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

extern Mpeg2encParam SVCDExtra,DVDExtra; 
extern COMPRES_PARAMS SVCDCodec,DVDCodec;

extern uint8_t audioShift;
extern int32_t audioDelay;
// IF set do 1st pass with CBR with max bitrate = average bitrate AND disabling padding
// else do 1st pass with constant Q=6
//#define ADM_1PASS_CBR 1
#define MAXAUDIO 56000

extern const char *getStrFromAudioCodec( uint32_t codec);

// ______________________________________________
// 								Initialise all variables
// ______________________________________________

mpegWritter::mpegWritter( void )
{	
	_w=_h=0;
	_fps1000=0;
	_page=0;
	_codec=NULL;
	aImage=NULL;
	_buffer_out=NULL;
	_audio=NULL;
	_audioBuffer=NULL;
	_muxer=NULL;
	_outputAs=MUXER_NONE;        
	audioWanted=audioGot=0;
	_ratecontrol=NULL;

}
mpegWritter::mpegWritter( ADM_MUXER_TYPE ps )
{	
	_w=_h=0;
	_fps1000=0;
	_page=0;
	_codec=NULL;
	aImage=NULL;
	_buffer_out=NULL;
	_audio=NULL;
	_audioBuffer=NULL;
	_muxer=NULL;
        _outputAs=ps;
        
	audioWanted=audioGot=0;
	_ratecontrol=NULL;

}
//_______________________________________
// Clean up in case we forgot something
//_______________________________________
mpegWritter::~mpegWritter(  )
{
    		end();

}
/*---------------------------------------------------------------------------------------*/
uint8_t  mpegWritter::save_svcd(char *name)
{
	if(_outputAs)
	{
		switch(_outputAs)
		{
                        
			case MUXER_SVCD:
				if(!initLveMux(name,MUXER_SVCD))
					return 0;
				break;
			
			default:
				ADM_assert(0);
				break;
		}
	}
	switch(SVCDCodec.mode)
	{
		case COMPRESS_CBR:
			return  save_regular(name, ADM_SVCD,
						0, 
						SVCDCodec.bitrate,
						SVCDExtra.user_matrix,
						SVCDExtra.interlaced,
    					SVCDExtra.bff,         // WLA
						SVCDExtra.widescreen
						);
			break;
		case COMPRESS_CQ:
			return  save_regular(name, ADM_SVCD,
					 	SVCDCodec.qz,
						8*SVCDExtra.maxBitrate,
						SVCDExtra.user_matrix,
						SVCDExtra.interlaced,
						SVCDExtra.bff,         // WLA
						SVCDExtra.widescreen
								);
			break;
		case COMPRESS_2PASS:
			return  save_dualpass(name,SVCDCodec.finalsize,
						8*SVCDExtra.maxBitrate,
						ADM_SVCD,
						SVCDExtra.user_matrix,
						SVCDExtra.interlaced,
						SVCDExtra.bff,         // WLA
						SVCDExtra.widescreen
								
								);
		break;
	}
	ADM_assert(0);
	return 0;
}



#define PACK_AUDIO \
{ \
	uint32_t samples; \
	while(_muxer->needAudio()) \
	{				\
		if(!_audio->getPacket(_audioBuffer, &audiolen, &samples))	\
		{ \
			break; \
		}\
		if(audiolen) \
		{\
			_muxer->writeAudioPacket(audiolen,_audioBuffer); \
			encoding->feedAudioFrame(audiolen); \
		}\
		audioGot+=audiolen; \
	} \
}
/*---------------------------------------------------------------------------------------*/
uint8_t  mpegWritter::save_dvd(char *name)
{
AVDMGenericAudioStream 	*tmp=NULL;
WAVHeader		*info=NULL,tmpinfo;	

	// look if we have a suitable audio
	if(_outputAs)
	{
		switch(_outputAs)
		{
                        case MUXER_TS:
                                if(!initLveMux(name,MUXER_TS))
                                        return 0;
                                break;
			case MUXER_DVD:
				if(!initLveMux(name,MUXER_DVD))
					return 0;
				break;
			
			default:
				ADM_assert(0);
				break;
		}
	}

	switch(DVDCodec.mode)
	{
		case COMPRESS_CBR:
			printf("Dvd encoding in CBR mode\n");
			return  save_regular(name, ADM_DVD,0, 
						DVDCodec.bitrate ,
						DVDExtra.user_matrix,
						DVDExtra.interlaced,
						DVDExtra.bff,         // WLA
						DVDExtra.widescreen
						);
			break;
		case COMPRESS_CQ:
			printf("Dvd encoding in CQ mode\n");
			return  save_regular(name, ADM_DVD,
						 DVDCodec.qz,
						 8*DVDExtra.maxBitrate ,
						 DVDExtra.user_matrix,
						 DVDExtra.interlaced,
						 DVDExtra.bff,         // WLA
						 DVDExtra.widescreen
						);
			break;
		case COMPRESS_2PASS:
			printf("Dvd encoding in 2pass mode\n");
			return  save_dualpass(name,
						DVDCodec.finalsize,
						8*DVDExtra.maxBitrate,
						ADM_DVD,
						DVDExtra.user_matrix,
						DVDExtra.interlaced,
						DVDExtra.bff,         // WLA
						DVDExtra.widescreen
						);
		break;
	}
	ADM_assert(0);
	return 0;
}
// ______________________________________________
// 										Do the actual save
// ______________________________________________

uint8_t  mpegWritter::save_vcd(char *name)
{

// look if we have a suitable audio
	if(_outputAs)
	{
		switch(_outputAs)
		{
			case MUXER_VCD:
				if(!initLveMux(name,MUXER_VCD))
					return 0;
				break;
			
			default:
				ADM_assert(0);
				break;
		}
	}
	return save_regular(name,ADM_VCD,1,1,0,0,0,0); // WLA

}
// /VCD

/*
	Save as with the external mpeg2enc

*/
uint8_t mpegWritter::save_regular(char *name, ADM_MPEGTYPE mpegtype, int qz, int bitrate,int matrix,
					uint8_t interlaced,
					uint8_t bff,        // WLA
					uint8_t widescreen)
{
 uint32_t 		size;
AVDMGenericVideoStream	 *incoming;

FILE			*fd=NULL;
uint64_t		total_size=0;
uint32_t		len,flags;
uint32_t 		outquant;
uint32_t		audiolen=0;
DIA_encoding		*encoding;

   	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	_total=incoming->getInfo()->nb_frames;
	_fps1000=incoming->getInfo()->fps1000;
	if(!_total) return 0;

	printf("Br:%d, qz:%d\n",bitrate,qz);
	if(!_audio)
	{
		if(!(fd=qfopen(name,"wb")))
			return 0;			
	}
	else
	{
		ADM_assert(_muxer);
		

	}


	_w=incoming->getInfo()->width;
	_h=incoming->getInfo()->height;
	

 	_page=_w*_h;
	_page+=_page>>1;

//			if(!init(name,ADM_VCD,interlaced,widescreen)) return 0;
	if(!init(name,ADM_VCD,interlaced,bff,widescreen)) return 0; //WLA
	printf("\n mpeg2enc init done \n");


	//_buffer	=new uint8_t[_w*_h*2];
	aImage=new ADMImage(_w,_h);
	_buffer_out=new uint8_t[_w*_h*2];

	ADM_assert(  aImage);
	ADM_assert(  _buffer_out);

	encoding=new DIA_encoding(_fps1000);

	encoding->setPhasis("Encoding.");
	encoding->setFrame(0,_total);
//	printf("Br:%d, qz:%d\n",bitrate,qz);

	switch(mpegtype)
	{
		case ADM_VCD:
					{
					encoding->setCodec("VCD.");
					Mpeg2encVCD *dec;
					dec=new Mpeg2encVCD(_w,_h);
//					dec->init(1,0,_fps1000,interlaced,widescreen);
					dec->init(1,0,_fps1000,interlaced,bff,widescreen,0); // WLA
					_codec=dec;
					}
					break;
		case ADM_SVCD:

					Mpeg2encSVCD *dec;
					dec=new Mpeg2encSVCD(_w,_h);
					dec->setMatrix(matrix);
//					dec->init(qz,bitrate,_fps1000,interlaced,widescreen);
					dec->init(qz,bitrate,_fps1000,interlaced,bff,widescreen,0); 
					// WLA
					_codec=dec;
					encoding->setCodec("SVCD.");

					break;

		case ADM_DVD:
				{
					Mpeg2encDVD *dec;
					dec=new Mpeg2encDVD(_w,_h);
					dec->setMatrix(matrix);
//					dec->init(qz,bitrate,_fps1000,interlaced,widescreen);
					dec->init(qz,bitrate,_fps1000,interlaced,bff,widescreen,0); 
					// WLA
					_codec=dec;
					encoding->setCodec("DVD.");

				}
					break;
		default: ADM_assert(0);
	}

	printf("\n--encoding started--\n");
	if(_muxer)
	{
            if(audioProcessMode())
                encoding->setAudioCodec(getStrFromAudioCodec(_audio->getInfo()->encoding));
            else
                encoding->setAudioCodec("Copy");
            switch(_outputAs)
            {
                
                case MUXER_TS:  encoding->setContainer("Mpeg TS");break;
                case MUXER_VCD: encoding->setContainer("Mpeg VCD");break;
                case MUXER_SVCD:encoding->setContainer("Mpeg SVCD");break;
                case MUXER_DVD: encoding->setContainer("Mpeg DVD");break;
                default:
                    ADM_assert(0);           
            }
            
	}
        else
            encoding->setContainer("Mpeg ES");
	for(uint32_t i=0;i<_total;i++)
			{
            			if(!incoming->getFrameNumberNoAlloc(i, &size,
					aImage,&flags))
				{
						delete encoding;
               					GUI_Error_HIG("Encoding error", NULL);
						if(fd)
							qfclose(fd);
						end();
						return 0 ;
				}
				_codec->encode(	aImage,_buffer_out , &len,&flags,&outquant);
				total_size+=len;
				encoding->feedFrame(len);
				encoding->setQuant(outquant);
				encoding->setFrame(i,_total);
				// Null frame are only possible
				// when in prefill state for mpeg-X
				if(!len) continue;
				if(_muxer)
				{		
						_muxer->writeVideoPacket(len,_buffer_out,
							i-MPEG_PREFILL,_codec->getCodedPictureNumber());
						PACK_AUDIO;
						
				}else
				{
					qfwrite(_buffer_out,len,1,fd);
					fflush(fd);
				}
									
				aprintf(" outquant %02d  size :%d flags %x\n",outquant,len,flags);
				
				if(!encoding->isAlive())
					{
						delete encoding;
						end();
						if(fd)
						  qfclose(fd);
						 return 0;
					}
			}
			encoding->setPhasis("Finishing");
			for(uint32_t i=0;i<MPEG_PREFILL;i++)
			{
				_codec->encode(aImage,_buffer_out , &len,&flags);
				total_size+=len;
				encoding->feedFrame(len);
				if(!_muxer)
					qfwrite(_buffer_out,len,1,fd);
				else
				{
					_muxer->writeVideoPacket(len,_buffer_out,
							_total-MPEG_PREFILL+i,_codec->getCodedPictureNumber());
					PACK_AUDIO;
				}

				//	printf("\n pipe opened %ld\n",i);
				encoding->setFrame(i,_total);

			}
			delete encoding;
			
			if(!_muxer)
				qfclose(fd);
			else
			{
				_muxer->close();
				delete _muxer;
				_muxer=NULL;
				deleteAudioFilter();
				_audio=NULL;				
			}
			end();
			return 1;

}


DIA_encoding *encoding;
/*-------------------------------------------------------------------------------------------------------------------*/
uint8_t  mpegWritter::save_dualpass(char *name,uint32_t final_size,uint32_t bitrate,ADM_MPEGTYPE mpegtype,
					int matrix,uint8_t interlaced,
					uint8_t bff,        // WLA
					uint8_t widescreen)
{
AVDMGenericVideoStream	 *incoming;
char *statname;
uint8_t reuse=0;

	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	_w=incoming->getInfo()->width;
	_h=incoming->getInfo()->height;
	_page=_w*_h;
	_page+=_page>>1;
	_fps1000=incoming->getInfo()->fps1000;
		_total=incoming->getInfo()->nb_frames;
	if(!_total) return 0;
	//_buffer	=new uint8_t[_w*_h*2];
	aImage=new ADMImage(_w,_h);
	_buffer_out=new uint8_t[_w*_h*2];


	statname=new char[strlen(name)+4+1];
	strcpy(statname,name);
	strcat(statname,".st");
	printf("Matrix : %d\n\n",matrix);
	encoding=new DIA_encoding(_fps1000);
	encoding->setPhasis("Encoding");


	// check if stat file exists ?
#if 1 //ndef CYG_MANGLING	
	{ FILE *fd;	
		fd=fopen(statname,"rt");
		if(fd){
			fclose(fd);
			prefs->get(FEATURE_REUSE_2PASS_LOG,(uint8_t*)&reuse);
			if( !reuse && GUI_Question("Reuse log file ?") ) reuse=1;
		}
	}
#endif	
	//_ratecontrol=new ADM_oldXvidRc(_fps1000,statname);
	_ratecontrol=new ADM_newXvidRc(_fps1000,statname);
	
#if 1
if(!reuse)
//	if(!dopass1(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,widescreen))
	if(!dopass1(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,bff,widescreen)) 
	// WLA
	{
		delete encoding;
		GUI_Error_HIG("Error in pass 1", NULL);
		delete [] statname;
		return 0;
	}
#endif
//	if(!dopass2(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,widescreen))
	if(!dopass2(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,bff,widescreen)) 
	// WLA
	{
		delete encoding;
		GUI_Error_HIG("Error in pass 2", NULL);
		delete [] statname;
		return 0;
	}
	delete encoding;
	delete [] statname;
	return 1;
}
/*-------------------------------------------------------------------------------------------------------*/
uint8_t  mpegWritter::dopass1(char *name,char *statname,uint32_t final_size,uint32_t bitrate,
				ADM_MPEGTYPE mpegtype,int matrix,uint8_t interlaced,
					uint8_t bff,        // WLA
					uint8_t widescreen)
{
uint32_t 						size;
AVDMGenericVideoStream	 *incoming;

FILE						*fd;
uint64_t						total_size=0;
uint32_t						len,flags,type,outquant;
float 						br;
uint32_t 						avg;

int intra,q;


   					

   	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);

//	if(!init(name,mpegtype,interlaced,widescreen)) return 0;
	if(!init(name,mpegtype,interlaced,bff,widescreen)) return 0; // WLA
	printf("\n mpeg2enc init done \n");

	ADM_assert(  aImage);
	ADM_assert(  _buffer_out);

	q=2; // q=2
	encoding->setPhasis("1st Pass");
	bitrate=0; // we dont care in pass 1 ...

 	

	ADM_assert(_ratecontrol->startPass1());

	switch(mpegtype)
	{
		case ADM_SVCD:
				Mpeg2encSVCD *dec;
				dec=new Mpeg2encSVCD(_w,_h);
				dec->setMatrix(matrix);
#ifdef ADM_1PASS_CBR						
				dec->disablePadding();	
				//dec->init(qz,bitrate,_fps1000,interlaced,widescreen);	
//				dec->init(0,avg*1000,_fps1000,interlaced,widescreen);
				dec->init(0,avg*1000,_fps1000,interlaced,bff,widescreen,1); // WLA
#else
//				dec->init(q,0,_fps1000,interlaced,widescreen);
				dec->init(q,0,_fps1000,interlaced,bff,widescreen,1); // WLA
#endif						
				 // 6 Megabits should be enough
				//dec->init(0,avg,_fps1000); // 6 Megabits should be enough
				_codec=dec;
				encoding->setCodec("SVCD");
				break;
		case ADM_DVD:
			{

				Mpeg2encDVD *dec;
				dec=new Mpeg2encDVD(_w,_h);
				dec->setMatrix(matrix);
						
#ifdef ADM_1PASS_CBR
				dec->disablePadding();
//						dec->init(0,avg*1000,_fps1000,interlaced,widescreen);
				dec->init(0,avg*1000,_fps1000,interlaced,bff,widescreen,1); // WLA
#else						
//				dec->init(q,0,_fps1000,interlaced,widescreen);
				dec->init(q,0,_fps1000,interlaced,bff,widescreen,1); // WLA
#endif						
				_codec=dec;
				encoding->setCodec("DVD");;
				}
				break;
		default:
				ADM_assert(0);
				break;
	}

	encoding->setFrame (0, _total);

	for(uint32_t i=0;i<_total;i++)
	{
		_codec->setQuantize(q);
        	if(!incoming->getFrameNumberNoAlloc(i, &size,aImage,&flags))
		{
        		GUI_Error_HIG("Encoding error", NULL);
			end();
			return 0 ;
		}
		if(i<MPEG_PREFILL)
				{

					_codec->encode(aImage,_buffer_out , &len,&flags,&outquant);
					continue;
				}
		_codec->encode(		aImage,_buffer_out , &len,&flags,&outquant);
		total_size+=len;

		
		aprintf("inquant  : %02d outquant :%02d Intra %d, len %lu\n",q,outquant,intra,len);

		ADM_rframe ftype;
		switch(flags)
		{
			case AVI_KEY_FRAME: 	ftype=RF_I;break;
			case AVI_B_FRAME: 	ftype=RF_B;break;
			default:		ftype=RF_P;break;
		}

		ADM_assert(_ratecontrol->logPass1(outquant,ftype,len));
		
		encoding->setFrame(i,_total);
		encoding->setQuant(outquant);
		encoding->feedFrame(len);
		if(!encoding->isAlive())
		{
				 end();
				 return 0;
		}
	}
	// flush queue
	for(uint32_t i=0;i<MPEG_PREFILL;i++)
	{
		_codec->encode(		aImage,_buffer_out , &len,&flags,&outquant);
		total_size+=len;

		
		aprintf("inquant  : %02d outquant :%02d Intra %d\n",q,outquant,intra);

		ADM_rframe ftype;
		switch(flags)
		{
			case AVI_KEY_FRAME: 	ftype=RF_I;break;
			case AVI_B_FRAME: 	ftype=RF_B;break;
			default:		ftype=RF_P;break;
		}
		ADM_assert(_ratecontrol->logPass1(outquant,ftype,len));

		encoding->setFrame(i,MPEG_PREFILL);
		encoding->setQuant(outquant);
		encoding->feedFrame(len);
		
	}
	// flush queue
	delete _codec;
	_codec=NULL;
	
	return 1;
}
/*--------------------------------------------------------------------*/
uint32_t quantstat[32]; /* 0-31 ; 0 and 1 are unused */
void print_quant_stat(const char *n){
  char *str = (char*)ADM_alloc( strlen(n) + 4 );
  unsigned int i=2,sum=0,total=0;
  FILE *fd;
   ADM_assert( str );
   strcpy(str,n);
   strcat(str,".qs");
   if( (fd=qfopen(str,"wb")) ){
      for(;i<32;i++){
         qfprintf(fd,"Quant % 2u: % 7u times\n",i,quantstat[i]);
         sum+=i*quantstat[i];
         total+=quantstat[i];
      }
      qfprintf(fd,"\nQuant over all: %2.2f\n",(float)sum/(float)total);
      qfclose(fd);
   }
   ADM_dealloc( str );
}
uint8_t  mpegWritter::dopass2(char *name,char *statname,uint32_t final_size,uint32_t bitrate,
				ADM_MPEGTYPE mpegtype,int matrix,uint8_t interlaced,
					uint8_t bff,        // WLA
					uint8_t widescreen)
{
int 			intra,q;
uint32_t 		size;
AVDMGenericVideoStream	*incoming;

FILE			*fd=NULL;
uint64_t		total_size=0;
uint32_t		len,flags,type,outquant,audiolen;

	memset(quantstat,0,32);

   	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	if(!_audio)
	{
		if( !(fd=qfopen(name,"wb")) )
			return 0;
	}

//	if(!init(name,mpegtype,interlaced,widescreen))
	if(!init(name,mpegtype,interlaced,bff,widescreen)) // WLA
	{
		printf("Mpeg2 init failed\n");
		 return 0;
	}
			printf("\n mpeg2enc init done \n");

	ADM_assert(  aImage);
	ADM_assert(  _buffer_out);
	encoding->reset();
	encoding->setFrame (0, _total);

/*-------------------- Pass 1 over, go to pass 2 --------------------------------*/
	
	ADM_assert(_ratecontrol->startPass2(final_size,_total));
	
	encoding->setPhasis("2nd Pass");
	q=2;

	//mpegvbr.maxAllowedBitrate=(bitrate*1000)>>3;//(bitrate*1000)>>3;
	//mpegvbr.maxAllowedBitrate=(9000*1000)>>3; // enable stuff in xvid


//->
	switch(mpegtype)
	{
		case ADM_SVCD:
						
			Mpeg2encSVCD *dec;
				dec=new Mpeg2encSVCD(_w,_h);
				dec->setMatrix(matrix);
//				dec->init(q,bitrate,_fps1000,interlaced,widescreen);
				dec->init(q,bitrate,_fps1000,interlaced,bff,widescreen,0); // WLA
				_codec=dec;
				encoding->setCodec("SVCD");
				printf("Svcd  max bitrate : %d\n",bitrate);
				break;
		case ADM_DVD:
			{
				
				Mpeg2encDVD *dec;
				dec=new Mpeg2encDVD(_w,_h);
				dec->setMatrix(matrix);
//			dec->init(q,bitrate,_fps1000,interlaced,widescreen);
				dec->init(q,bitrate,_fps1000,interlaced,bff,widescreen,0); // WLA
				_codec=dec;
				printf("DVD  max bitrate : %d\n",bitrate);
				encoding->setCodec("DVD");
			}
			break;
		default:
				ADM_assert(0);
				break;
	}
	encoding->setPhasis("2nd Pass");
	if(_muxer)
	{
		encoding->setAudioCodec(getStrFromAudioCodec(_audio->getInfo()->encoding));	
	}
	for(uint32_t i=0;i<_total;i++)
	{
        	if(!incoming->getFrameNumberNoAlloc(i, &size,aImage,&flags))
		{
               		GUI_Error_HIG("Encoding error", NULL);
			if(!_audio)
				qfclose(fd);
			end();
			return 0 ;
		}
		encoding->setFrame(i,_total);

		if(i<MPEG_PREFILL)
		{

			_codec->encode(aImage,_buffer_out , &len,&flags,&outquant);	
			quantstat[outquant]++;
			continue;
		}
		encoding->feedFrame(len); // Set
		//
		ADM_rframe ftype,ztype;
		uint32_t qz;
		ADM_assert(_ratecontrol->getQz(&qz,&ztype));
		q=qz;
		_codec->setQuantize(q);
		_codec->encode(aImage,_buffer_out , &len,&flags,&outquant);
		quantstat[outquant]++;
		encoding->setQuant(outquant);
		
		switch(flags)
		{
			case AVI_KEY_FRAME: 	ftype=RF_I;break;
			case AVI_B_FRAME: 	ftype=RF_B;break;
			default:		ftype=RF_P;break;
		}
		if(ftype!=ztype)
		{
			printf("**Frame type does not match %d %d\n",ztype,ftype);
		}
		aprintf("inquant  : %02d outquant %02d Intra %d size :%d flags %x\n",
				q,outquant,intra,len,flags);
		ADM_assert(_ratecontrol->logPass2(outquant,ftype,len));
		

		total_size+=len;
	
		if(!_muxer)
			{			
				qfwrite(_buffer_out,len,1,fd);
				fflush(fd);
			}
		else
			{
				
				// write video
				_muxer->writeVideoPacket(len,_buffer_out,i-MPEG_PREFILL,_codec->getCodedPictureNumber());
						
				PACK_AUDIO;
			}
		
				
				
		if(!encoding->isAlive())
		{
			 print_quant_stat(name);
			 end();
			 qfclose(fd);
			 return 0;
		}
	}
			
//--
// flush queue
	for(uint32_t i=0;i<MPEG_PREFILL;i++)
	{
		
		ADM_rframe ftype;
		uint32_t qz;
		ADM_assert(_ratecontrol->getQz(&qz,&ftype));
		q=qz;
		_codec->setQuantize(q);
		_codec->encode(		aImage,_buffer_out , &len,&flags,&outquant);
		quantstat[outquant]++;
		encoding->setQuant(outquant);
		switch(flags)
		{
			case AVI_KEY_FRAME: 	ftype=RF_I;break;
			case AVI_B_FRAME: 	ftype=RF_B;break;
			default:		ftype=RF_P;break;
		}
		aprintf("inquant  : %02d outquant %02d Intra %d size :%d flags %x\n",
				q,outquant,intra,len,flags);
		ADM_assert(_ratecontrol->logPass2(outquant,ftype,len));

		total_size+=len;
		if(!_muxer)
		{		
			qfwrite(_buffer_out,len,1,fd);
			fflush(fd);
		}
		else
		{
			// write video
			_muxer->writeVideoPacket(len,_buffer_out,_total+i-MPEG_PREFILL,_codec->getCodedPictureNumber());
			PACK_AUDIO;

		}
		
		//	printf("\n pipe opened %ld\n",i);
		encoding->feedFrame(len); // Set
		encoding->setFrame(i,MPEG_PREFILL);
		
	}
//--			
	if(!_muxer)	
		qfclose(fd);
	else
	{
		_muxer->close();
		delete _muxer;
		_muxer=NULL;	
	}

	print_quant_stat(name);
	 end();
	return 1;
}


// ______________________________________________
//	Construct mpeg2enc string parameter
// ______________________________________________

uint8_t mpegWritter::init(char *name,ADM_MPEGTYPE type,uint8_t interlaced,uint8_t bff,uint8_t widescreen) // WLA
{
	UNUSED_ARG(name);
	UNUSED_ARG(type);
       if(! 	identMovieType(_fps1000))
	{
               		GUI_Error_HIG("Incompatible frame rate", NULL);
			return 0;
	}

	return 1;
}

// ______________________________________________
//						Clean -up
// ______________________________________________
uint8_t mpegWritter::end( void )
{
	if(_codec) delete _codec;
	_codec=NULL;
	// grab last bytes

	if(aImage) delete  aImage;
	if(_buffer_out) delete [] _buffer_out;
	aImage=NULL;
	_buffer_out=NULL;
	
	if(_audioBuffer)
	{
		delete [] _audioBuffer;
		_audioBuffer=NULL;
	}
	if(_muxer)
	{
		_muxer->close();
		delete _muxer;
		_muxer=NULL;
	}
	if(_ratecontrol)
	{
		delete _ratecontrol;
		_ratecontrol=NULL;
	}
      return 1;
}

// keep gcc happy when this is empty
int dummy_func_mpeg( int i )
{
	return i % 4;

}
//______________________________________
//
//	Set up the audio chain for audio+video writting
//
uint8_t mpegWritter::initLveMux( char *name,ADM_MUXER_TYPE type )
{
double one_pcm_audio_frame;
uint32_t fps1000;

	_audio=mpt_getAudioStream();
	if(!_audio)
	{
		printf("Cannot initialize lvemux\n");
		return 0;
	}
	fps1000=getLastVideoFilter()->getInfo()->fps1000;
	
  	
  	
	_audioBuffer=new uint8_t[MAXAUDIO]; // equivalent to 1 sec @ 448 kbps, should be more than
					// enough, even with the buffering
	printf("----- Audio Track for mpeg Ready.------\n");

        if(type!=MUXER_TS)
	       _muxer=new mplexMuxer();	
        else
               _muxer=new tsMuxer(); //lavMuxer();
        
	// open( char *filename, uint32_t vbitrate, aviInfo *info, WAVHeader *audioheader,float need);
	aviInfo info;
	info.width=getLastVideoFilter()->getInfo()->width;
	info.height=getLastVideoFilter()->getInfo()->height;
	info.fps1000=getLastVideoFilter()->getInfo()->fps1000;
	if(!_muxer->open(name,0,type,&info,_audio->getInfo()))
	{
		delete _muxer;
		_muxer=NULL;
		printf("Muxer init failed\n");
		return 0;
		
	}
	printf("Muxer ready\n");
  	return 1;	
}


AVDMGenericAudioStream *mpt_getAudioStream(void)
{
AVDMGenericAudioStream *audio=NULL;
	if (audioProcessMode())	// else Raw copy mode
	{
		if (currentaudiostream->isCompressed ())
		{
			if (!currentaudiostream->isDecompressable ())
			{
				return NULL;
			}
		}
	      	audio = buildAudioFilter (currentaudiostream,video_body->getTime (frameStart),
				  (uint32_t) 0xffffffff);
	}
  	else // copymode
	{
	// else prepare the incoming raw stream
	// audio copy mode here
	int32_t shift=0;
	if(audioDelay && audioShift) shift=audioDelay;
	audio=buildRawAudioFilter( video_body->getTime (frameStart), 
		0xffffffff, shift);
	}
	return audio;
}
// EOF

