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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


#include "config.h"

#include "ADM_lavcodec.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
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

extern "C" {
#include "ADM_encoder/xvid_vbr.h"
};
#include "ADM_dialog/DIA_enter.h"

static vbr_control_t mpegvbr;

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG2ENC
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_dialog/DIA_encoding.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"


#warning FIXME: Duplicate define with mpeg2enc -> bad
#define MPEG_PREFILL 29

extern FFMPEGConfig mpeg2encSVCDConfig;
extern FFMPEGConfig mpeg2encDVDConfig;
extern uint8_t audioShift;
extern int32_t audioDelay;
// IF set do 1st pass with CBR with max bitrate = average bitrate AND disabling padding
// else do 1st pass with constant Q=6
#define ADM_1PASS_CBR 1
#define MAXAUDIO 56000
// ______________________________________________
// 								Initialise all variables
// ______________________________________________

mpegWritter::mpegWritter( void )
{

	_w=_h=0;
	_fps1000=0;
	_page=0;
	_codec=NULL;
	_buffer=NULL;
	_buffer_out=NULL;
	_audio=NULL;
	_audioFifo=NULL;
	_videoFifo=NULL;
	_lvepipe=NULL;
	_audioBuffer=NULL;
	_outputAsPs=0;

}
mpegWritter::mpegWritter( uint8_t ps )
{

	_w=_h=0;
	_fps1000=0;
	_page=0;
	_codec=NULL;
	_buffer=NULL;
	_buffer_out=NULL;
	_audio=NULL;
	_audioFifo=NULL;
	_videoFifo=NULL;
	_lvepipe=NULL;
	_audioBuffer=NULL;
	_outputAsPs=1;

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

	switch(mpeg2encSVCDConfig.generic.mode)
	{
		case COMPRESS_CBR:
			return  save_regular(name, ADM_SVCD,
						0, 
						mpeg2encSVCDConfig.generic.bitrate,
						mpeg2encSVCDConfig.specific.user_matrix,
						mpeg2encSVCDConfig.specific.interlaced,
    						mpeg2encSVCDConfig.specific.bff,         // WLA
						mpeg2encSVCDConfig.specific.widescreen
						);
			break;
		case COMPRESS_CQ:
			return  save_regular(name, ADM_SVCD,
					 	mpeg2encSVCDConfig.generic.qz,
						8*mpeg2encSVCDConfig.specific.maxBitrate,
						mpeg2encSVCDConfig.specific.user_matrix,
						mpeg2encSVCDConfig.specific.interlaced,
						mpeg2encSVCDConfig.specific.bff,         // WLA
						mpeg2encSVCDConfig.specific.widescreen
								);
			break;
		case COMPRESS_2PASS:
			return  save_dualpass(name,mpeg2encSVCDConfig.generic.finalsize,
						8*mpeg2encSVCDConfig.specific.maxBitrate,
						ADM_SVCD,
						mpeg2encSVCDConfig.specific.user_matrix,
						mpeg2encSVCDConfig.specific.interlaced,
						mpeg2encSVCDConfig.specific.bff,         // WLA
						mpeg2encSVCDConfig.specific.widescreen
								);
		break;
	}
	assert(0);
	return 0;
}
/*---------------------------------------------------------------------------------------*/
uint8_t  mpegWritter::save_dvd(char *name)
{
AVDMGenericAudioStream 	*tmp=NULL;
WAVHeader		*info=NULL,tmpinfo;	

	// look if we have a suitable audio
	if(_outputAsPs)
	{
		if(!initLveMux(name))
		{
			return 0;
		}				
	}

	switch(mpeg2encDVDConfig.generic.mode)
	{
		case COMPRESS_CBR:
			printf("Dvd encoding in CBR mode\n");
			return  save_regular(name, ADM_DVD,0, 
						mpeg2encDVDConfig.generic.bitrate ,
						mpeg2encDVDConfig.specific.user_matrix,
						mpeg2encDVDConfig.specific.interlaced,
						mpeg2encDVDConfig.specific.bff,         // WLA
						mpeg2encDVDConfig.specific.widescreen
						);
			break;
		case COMPRESS_CQ:
			printf("Dvd encoding in CQ mode\n");
			return  save_regular(name, ADM_DVD,
						 mpeg2encDVDConfig.generic.qz,
						 8*mpeg2encDVDConfig.specific.maxBitrate ,
						 mpeg2encDVDConfig.specific.user_matrix,
						 mpeg2encDVDConfig.specific.interlaced,
						mpeg2encDVDConfig.specific.bff,         // WLA
						mpeg2encDVDConfig.specific.widescreen
						);
			break;
		case COMPRESS_2PASS:
			printf("Dvd encoding in 2pass mode\n");
			return  save_dualpass(name,
						mpeg2encDVDConfig.generic.finalsize,
						8*mpeg2encDVDConfig.specific.maxBitrate,
						ADM_DVD,
						mpeg2encDVDConfig.specific.user_matrix,
						mpeg2encDVDConfig.specific.interlaced,
						mpeg2encDVDConfig.specific.bff,         // WLA
						mpeg2encDVDConfig.specific.widescreen
						);
		break;
	}
	assert(0);
	return 0;
}
// ______________________________________________
// 										Do the actual save
// ______________________________________________

uint8_t  mpegWritter::save_vcd(char *name)
{
//	return save_regular(name,ADM_VCD,1,1,0,0,0);
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

DIA_encoding		*encoding;

   	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	_total=incoming->getInfo()->nb_frames;
	_fps1000=incoming->getInfo()->fps1000;
	if(!_total) return 0;

	printf("Br:%d, qz:%d\n",bitrate,qz);
	if(!_audio)
	{
		fd=fopen(name,"wb");
		if(!fd)
		{
			GUI_Alert("Problem opening file!");
			return 0;			
		}
	}
	else
	{
		assert(_audioOneFrame);
		assert(_audioBuffer);
		assert(_audioFifo);
		assert(_videoFifo);

	}


	_w=incoming->getInfo()->width;
	_h=incoming->getInfo()->height;
	

 	_page=_w*_h;
	_page+=_page>>1;

//			if(!init(name,ADM_VCD,interlaced,widescreen)) return 0;
	if(!init(name,ADM_VCD,interlaced,bff,widescreen)) return 0; //WLA
	printf("\n mpeg2enc init done \n");


	_buffer	=new uint8_t[_w*_h*2];
	_buffer_out=new uint8_t[_w*_h*2];

	assert(  _buffer);
	assert(  _buffer_out);

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
					dec->init(1,0,_fps1000,interlaced,bff,widescreen); // WLA
					_codec=dec;
					}
					break;
		case ADM_SVCD:

					Mpeg2encSVCD *dec;
					dec=new Mpeg2encSVCD(_w,_h);
					dec->setMatrix(matrix);
//					dec->init(qz,bitrate,_fps1000,interlaced,widescreen);
					dec->init(qz,bitrate,_fps1000,interlaced,bff,widescreen); 
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
					dec->init(qz,bitrate,_fps1000,interlaced,bff,widescreen); 
					// WLA
					_codec=dec;
					encoding->setCodec("DVD.");

				}
					break;
		default: assert(0);
	}

	printf("\n--encoding started--\n");
	for(uint32_t i=0;i<_total;i++)
			{
            			if(!incoming->getFrameNumberNoAlloc(i, &size,
					(uint8_t *) _buffer,&flags))
				{
						delete encoding;
               					GUI_Alert("Encoding error !");
						if(fd)
							fclose(fd);
						end();
						return 0 ;
				}
				_codec->encode(	_buffer,_buffer_out , &len,&flags,&outquant);
				total_size+=len;
				encoding->feedFrame(len);
				encoding->setQuant(outquant);
				encoding->setFrame(i,_total);
				// Null frame are only possible
				// when in prefill state for mpeg-X
				if(!len) continue;
				if(_audio)
				{		
				uint32_t audiolen;		
						// write video
						fwrite(_buffer_out,len,1,_videoFifo);
						fflush(_videoFifo);
						audiolen=_audioOneFrame;
						audiolen = _audio->read (audiolen,_audioBuffer);
						assert(audiolen<MAXAUDIO);
						fwrite(_audioBuffer,audiolen,1,_audioFifo);	
						fflush(_audioFifo);
						aprintf("Audio :%lu Video:%lu (%lu)\n",audiolen,len,
							_audioOneFrame);
				}else
				{
					fwrite(_buffer_out,len,1,fd);
					fflush(fd);
				}
									
				aprintf(" outquant %02d  size :%d flags %x\n",outquant,len,flags);
				
				if(!encoding->isAlive())
					{
						delete encoding;
						end();
						if(fd)
						  fclose(fd);
						 return 0;
					}
			}
			encoding->setPhasis("Finishing");
			for(uint32_t i=0;i<MPEG_PREFILL;i++)
			{
				_codec->encode(_buffer,_buffer_out , &len,&flags);
				total_size+=len;
				encoding->feedFrame(len);
				if(!_audio)
					fwrite(_buffer_out,len,1,fd);
				else
				{
					fwrite(_buffer_out,len,1,_videoFifo);
					len=_audioOneFrame;
					len = _audio->read (len, _audioBuffer);
					fwrite(_audioBuffer,len,1,_audioFifo);
				}

				//	printf("\n pipe opened %ld\n",i);
				encoding->setFrame(i,_total);

			}
			delete encoding;
			
			if(!_audio)
				fclose(fd);
			else
			{
				fflush(_audioFifo);
				fflush(_videoFifo);
				if(_audioFifo)
				{
					fclose(_audioFifo);
					_audioFifo=NULL;
				}
				if(_videoFifo)
				{
					fclose(_videoFifo);
					_videoFifo=NULL;
				}
				if(_lvepipe)
				{
					printf("Killing lvemux (main loop)\n");
				 	pclose(_lvepipe);
			 		_lvepipe=NULL;
				}
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

	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	_w=incoming->getInfo()->width;
	_h=incoming->getInfo()->height;
	_page=_w*_h;
	_page+=_page>>1;
	_fps1000=incoming->getInfo()->fps1000;
		_total=incoming->getInfo()->nb_frames;
	if(!_total) return 0;
	_buffer	=new uint8_t[_w*_h*2];
	_buffer_out=new uint8_t[_w*_h*2];


	statname=new char[strlen(name)+4+1];
	strcpy(statname,name);
	strcat(statname,".st");
	printf("Matrix : %d\n\n",matrix);
	encoding=new DIA_encoding(_fps1000);
	encoding->setPhasis("Encoding");


	// check if stat file exists ?
	FILE *fd;
	uint8_t reuse=0;
	fd=fopen(statname,"rt");
	if(fd)
	{
		fclose(fd);
		if(GUI_Question("Reuse log file ?")) reuse=1;
	}
#if 1
if(!reuse)
//	if(!dopass1(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,widescreen))
	if(!dopass1(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,bff,widescreen)) 
	// WLA
	{
		delete encoding;
		GUI_Alert("Error in pass 1");
		delete [] statname;
		return 0;
	}
#endif
//	if(!dopass2(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,widescreen))
	if(!dopass2(name,statname,final_size,bitrate,mpegtype,matrix,interlaced,bff,widescreen)) 
	// WLA
	{
		delete encoding;
		GUI_Alert("Error in pass 2");
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

	assert(  _buffer);
	assert(  _buffer_out);

	q=6; // q=2
	encoding->setPhasis("1st Pass");
	bitrate=0; // we dont care in pass 1 ...

 	memset(&mpegvbr,0,sizeof(mpegvbr));
       	if(0>  vbrSetDefaults(&mpegvbr)) 				return 0;
	mpegvbr.fps=_fps1000/1000.;

 	mpegvbr.mode=VBR_MODE_2PASS_1;
	mpegvbr.desired_size=final_size*1024*1024;
	mpegvbr.debug=0;
	mpegvbr.filename=statname; //XvidInternal2pass_statfile;

					
	br=mpegvbr.desired_size*8;
	br=br/_total;				// bit / frame
	br=br*mpegvbr.fps;
	br/=1000;
	avg=(uint32_t)floor(br);

	printf("Average bitrate : %lu\n",avg);

	if(0>   vbrInit(&mpegvbr))             				return 0;

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
				dec->init(0,avg*1000,_fps1000,interlaced,bff,widescreen); // WLA
#else
//				dec->init(q,0,_fps1000,interlaced,widescreen);
				dec->init(q,0,_fps1000,interlaced,bff,widescreen); // WLA
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
				dec->init(0,avg*1000,_fps1000,interlaced,bff,widescreen); // WLA
#else						
//				dec->init(q,0,_fps1000,interlaced,widescreen);
				dec->init(q,0,_fps1000,interlaced,bff,widescreen); // WLA
#endif						
				_codec=dec;
				encoding->setCodec("DVD");;
				}
				break;
		default:
				assert(0);
				break;
	}

	encoding->setFrame (0, _total);

	for(uint32_t i=0;i<_total;i++)
	{
		_codec->setQuantize(q);
        	if(!	incoming->getFrameNumberNoAlloc(i, &size,(uint8_t *) _buffer,&flags))
		{
        		GUI_Alert("Encoding error !");
			end();
			return 0 ;
		}
		if(i<MPEG_PREFILL)
				{

					_codec->encode(_buffer,_buffer_out , &len,&flags,&outquant);
					continue;
				}
		_codec->encode(		_buffer,_buffer_out , &len,&flags,&outquant);
		total_size+=len;

		if(flags & AVI_KEY_FRAME) intra=1;
			else					intra=0;
		aprintf("inquant  : %02d outquant :%02d Intra %d, len %lu\n",q,outquant,intra,len);

		switch(flags)
		{
			case AVI_KEY_FRAME: 	type=1;
						break;
			case AVI_B_FRAME: 	type=3;
						break;
			default:
						type=2;
						break;
		}

		vbrUpdate(&mpegvbr,
				outquant,
				intra,
				0,
				len,
				0,
				0,
				type);
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
		_codec->encode(		_buffer,_buffer_out , &len,&flags,&outquant);
		total_size+=len;

		if(flags & AVI_KEY_FRAME) intra=1;
			else					intra=0;
		aprintf("inquant  : %02d outquant :%02d Intra %d\n",q,outquant,intra);

		switch(flags)
		{
			case AVI_KEY_FRAME: 	type=1;
									break;
			case AVI_B_FRAME: 	type=3;
									break;
			default:
								type=2;
								break;
		}
		vbrUpdate(&mpegvbr,
				outquant,
				intra,
				0,
				len,
				0,
				0,
				type);
		encoding->setFrame(i,MPEG_PREFILL);
		encoding->setQuant(outquant);
		encoding->feedFrame(len);
		
	}
	// flush queue
	delete _codec;
	_codec=NULL;
	vbrFinish(&mpegvbr);
	return 1;
}
/*--------------------------------------------------------------------*/
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
uint32_t		len,flags,type,outquant;


   	incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	if(!_audio)
	{
		fd=fopen(name,"wb");
		if(!fd)
		{
			GUI_Alert("Problem opening file!");
			return 0;

		}
	}

//	if(!init(name,mpegtype,interlaced,widescreen))
	if(!init(name,mpegtype,interlaced,bff,widescreen)) // WLA
	{
		printf("Mpeg2 init failed\n");
		 return 0;
	}
			printf("\n mpeg2enc init done \n");

	assert(  _buffer);
	assert(  _buffer_out);
	encoding->reset();
	encoding->setFrame (0, _total);

/*-------------------- Pass 1 over, go to pass 2 --------------------------------*/
	total_size=0;
	 memset(&mpegvbr,0,sizeof(mpegvbr));
	if(0>  vbrSetDefaults(&mpegvbr))
	{
		printf("set default failed\n");
		return 0;
	}
	mpegvbr.fps=_fps1000/1000.;

	mpegvbr.mode=VBR_MODE_2PASS_2;
	mpegvbr.desired_size=final_size*1024*1024;
	mpegvbr.debug=0;
	mpegvbr.filename=statname; //XvidInternal2pass_statfile;

	float br;
	uint32_t avg;

	br=mpegvbr.desired_size*8;
	br=br/_total;				// bit / frame
	br=br*mpegvbr.fps;

	mpegvbr.desired_bitrate= (int)floor(br);
	avg=(uint32_t)floor(br/1000.);
	printf("average bitrate : %lu\n",avg);
	//mpegvbr.maxAllowedBitrate=(2500*1000)>>3; 
	// enable stuff in xvid
	//mpegvbr.twopass_max_bitrate=2500*1000;
	//mpegvbr.alt_curve_high_dist=10;
	//mpegvbr.alt_curve_low_dist=30;
	//mpegvbr.alt_curve_type=VBR_ALT_CURVE_AGGRESIVE;

	if(0>vbrInit(&mpegvbr))
	{
		GUI_Alert("Error initializing 2 pass!");
		return 0;
	}
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
				dec->init(q,bitrate,_fps1000,interlaced,bff,widescreen); // WLA
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
				dec->init(q,bitrate,_fps1000,interlaced,bff,widescreen); // WLA
				_codec=dec;
				printf("DVD  max bitrate : %d\n",bitrate);
				encoding->setCodec("DVD");
			}
			break;
		default:
				assert(0);
				break;
	}
	encoding->setPhasis("2nd Pass");
	for(uint32_t i=0;i<_total;i++)
	{
        	if(!incoming->getFrameNumberNoAlloc(i, &size,(uint8_t *) _buffer,&flags))
		{
               		GUI_Alert("Encoding error !");
			if(!_audio)
				fclose(fd);
			end();
			return 0 ;
		}
		encoding->setFrame(i,_total);

		if(i<MPEG_PREFILL)
		{

			_codec->encode(_buffer,_buffer_out , &len,&flags,&outquant);	
			continue;
		}
		encoding->feedFrame(len); // Set
		/* prepare quantization for next gop */
		//if(vbr_next_intra( &mpegvbr))
		{
			q=vbrGetQuant(&mpegvbr);
			_codec->setQuantize(q);
		}
		_codec->encode(_buffer,_buffer_out , &len,&flags,&outquant);
		encoding->setQuant(outquant);
		if(flags & AVI_KEY_FRAME) intra=1;
		else			intra=0;
		switch(flags)
		{
			case AVI_KEY_FRAME: 	type=1;
																		break;
			case AVI_B_FRAME: 	type=3;
																		break;
			default:
						type=2;
						break;
		}
		aprintf("inquant  : %02d outquant %02d Intra %d size :%d flags %x\n",
				q,outquant,intra,len,flags);
		vbrUpdate(&mpegvbr,
				outquant,
				//q,
				intra,
				0,
				len,
				0,
				0,
				type);

		total_size+=len;
	
		if(!_audio)
			{			
				fwrite(_buffer_out,len,1,fd);
				fflush(fd);
			}
		else
			{
				fwrite(_buffer_out,len,1,_videoFifo);
				fflush(_videoFifo);
				len=_audioOneFrame;
				len = _audio->read (len, _audioBuffer);
				fwrite(_audioBuffer,len,1,_audioFifo);		
				fflush(_audioFifo);
			}
		
				
				
		if(!encoding->isAlive())
		{
			 end();
			 fclose(fd);
			 return 0;
		}
	}
			
//--
// flush queue
	for(uint32_t i=0;i<MPEG_PREFILL;i++)
	{
		
		q=vbrGetQuant(&mpegvbr);
		_codec->setQuantize(q);
		_codec->encode(		_buffer,_buffer_out , &len,&flags,&outquant);
		encoding->setQuant(outquant);
		if(flags & AVI_KEY_FRAME) intra=1;
			else		intra=0;
		switch(flags)
		{
			case AVI_KEY_FRAME: 	type=1;
						break;
			case AVI_B_FRAME: 	type=3;
						break;
			default:
						type=2;
						break;
		}		

		printf("inquant  : %02d outquant %02d Intra %d size :%d flags %x\n",q,outquant,
					intra,len,flags);
		vbrUpdate(&mpegvbr,
				outquant,
				//q,
				intra,
				0,
				len,
				0,
				0,
				type);

		total_size+=len;
		if(!_audio)
		{		
			fwrite(_buffer_out,len,1,fd);
			fflush(fd);
		}
		else
		{
			fwrite(_buffer_out,len,1,_videoFifo);
			fflush(_videoFifo);
			len=_audioOneFrame;
			len = _audio->read (len, _audioBuffer);
			fwrite(_audioBuffer,len,1,_audioFifo);
			fflush(_audioFifo);
		}
		
		//	printf("\n pipe opened %ld\n",i);
		encoding->feedFrame(len); // Set
		encoding->setFrame(i,MPEG_PREFILL);
		
	}
//--			
	if(!_audio)	
		fclose(fd);
	else
	{
		fflush(_audioFifo);
		fflush(_videoFifo);
		if(_audioFifo)
		{
			fclose(_audioFifo);
			_audioFifo=NULL;
		}
		if(_videoFifo)
		{
			fclose(_videoFifo);
			_videoFifo=NULL;
		}
		if(_lvepipe)
			{
				printf("Killing lvemux (main loop)\n");
			 	pclose(_lvepipe);
		 		_lvepipe=NULL;
			}				
	}

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
               		GUI_Alert("Frame rate incompatible !");
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

	if(_buffer) delete [] _buffer;
	if(_buffer_out) delete [] _buffer_out;
	_buffer=_buffer_out=NULL;
	
	if(_audioBuffer)
	{
		delete [] _audioBuffer;
		_audioBuffer=NULL;
	}
	if(_audioFifo)
	{
		fflush(_audioFifo);
		fclose(_audioFifo);
		_audioFifo=NULL;
	}
	if(_videoFifo)
	{
		fflush(_videoFifo);
		fclose(_videoFifo);
		_videoFifo=NULL;
	}
	if(_lvepipe)
	{
		 pclose(_lvepipe);
		 _lvepipe=NULL;
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
uint8_t mpegWritter::initLveMux( char *name )
{
	char *lvemux;
	char cmd[256];
	uint32_t fps1000;
	uint32_t one_pcm_audio_frame;
	int err;
	
#define VIDEOF "/tmp/videofifo"	
#define AUDIOF "/tmp/audiofifo"

	if(!prefs->get(LVEMUX_PATH, &lvemux) || !strlen(lvemux))
		{
			GUI_Alert("Use misc->prefs to tell where lvemux is\n");
			return 0;
		}
	printf("----- Preparing audio track.------\n");
	// open audio and video fifo
		err=mkfifo(AUDIOF, S_IREAD+S_IWRITE);
		
		if(err && EEXIST!=errno)
		{
			GUI_Alert("Trouble creating  "AUDIOF);
			return 0;
		}
		err=mkfifo(VIDEOF, S_IREAD+S_IWRITE);
		if(err && EEXIST!=errno)
		{
			GUI_Alert("Trouble creating  "VIDEOF);
			return 0;
		}
		printf("Opening audio\n");
		_audioFifo=fopen(AUDIOF,"w+");
		if(!_audioFifo)
		{
			GUI_Alert("Trouble opening  "AUDIOF);
			return 0;
		}
		printf("Opening video\n");
		_videoFifo=fopen(VIDEOF,"w+");
		if(!_videoFifo)
		{
			GUI_Alert("Trouble opening  "VIDEOF);
			return 0;
		}
	// Try to open lvemux
		sprintf(cmd,"%s -p -v "VIDEOF" -a "AUDIOF" -o %s",lvemux,name);
		free(lvemux);
		printf("Spawning lvemux with cmd : *%s*\n",cmd);
		_lvepipe=popen(cmd,"w");
	// open lvemux
		if(!_lvepipe)
		{
			GUI_Alert("Problem spawning lvemux");
			printf("%d errno\n",errno);
			perror(NULL);
			return 0;			
		}
	// ________________prepare audio________________________
	    
     
      	// compute the number of bytes in the incoming stream
      	// to feed the filter chain
      	double	byt;
      	byt =	video_body->getTime (frameEnd + 1) - video_body->getTime (frameStart);
      	byt *= currentaudiostream->getInfo ()->frequency;
      	byt *= currentaudiostream->getInfo ()->channels;
      	byt *= 2;
      	byt /= 1000.;

	if(audioProcessMode)
	{
      		_audio = buildAudioFilter (currentaudiostream,video_body->getTime (frameStart),
				  (uint32_t) floor (byt));
	}
	else
	{
	
        	uint32_t    tstart;
	  	tstart = video_body->getTime (frameStart);
		_audio = (AVDMGenericAudioStream *) currentaudiostream;
	  	_audio->goToTime (tstart);	  
	}
   	
	//________________
	uint32_t one_frame;
  	aviInfo   info;
	double    one_frame_double, one_delta_frame;
  	WAVHeader *wav = NULL;

  	assert (_audio);

  	wav = _audio->getInfo ();
  
  	video_body->getVideoInfo (&info);

  	// In case of process, we must take
  	// the output fps in case the filter alters
  	// the fps. Else we would end up with a badly muxed
  	// file as the audio and video won't have the same 
  	// clockrate.
  	if(1) //_videoProcess)
  	{
		ADV_Info *lastInfo;
  		lastInfo=getLastVideoFilter()->getInfo ();
		fps1000=lastInfo->fps1000;
  	} // else in copy mode we take it from source
  	else
  	{
  		fps1000 = info.fps1000;
  	}
  	// compute duration of a audio frame
  	// in ms
  	assert (fps1000);
  	printf (" fps : %lu\n", fps1000);
  	one_frame_double = (double) fps1000;
  	one_frame_double = 1. / one_frame_double;
  	// now we have 1/1000*fps=1/1000*duration of a frame in second
  	one_frame_double *= 1000000.;
  	// in ms now;
  	one_frame = (uint32_t) floor (one_frame_double);
  	printf (" One audio frame : %lu ms\n", one_frame);


  	double    pcm;
  	// *2 because one sample is 16 bits
  	// fix hitokiri bug part 1.
  	pcm = one_frame_double * 2 * wav->frequency * wav->channels;
  	pcm /= 1000;
  	one_pcm_audio_frame = (uint32_t) floor (pcm);
  	printf (" one PCM audio frame is %lu bytes \n", one_pcm_audio_frame);

  	// get the equivalent in bytes
  	assert (wav);
  	one_frame_double /= 1000.;	// go back to seconds
  	one_frame_double *= wav->byterate;


	  one_frame = (uint32_t) floor (one_frame_double);

  	if (one_frame & 1)
    		one_frame--;
  	one_delta_frame = one_frame_double - one_frame;
  	// Real ? correction of hitokiri bug
  	one_delta_frame *= 1000;
  	printf (" One audio frame : %lu bytes\n", one_frame);
  	_audioOneFrame=one_frame;
	_audioBuffer=new uint8_t[MAXAUDIO]; // equivalent to 1 sec @ 448 kbps, should be more than
					// enough, even with the buffering
	printf("----- Audio Track for mpeg Ready.------\n");
  	return 1;	
}
