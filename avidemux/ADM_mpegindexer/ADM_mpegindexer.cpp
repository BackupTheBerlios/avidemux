/***************************************************************************
                          ADM_mpegindexer.cpp  -  description
                             -------------------
		Mix between mpeg2dec code, vdub one and mine.         
		
		The audio stream is a bit complicated
		if C0 C1 ... mpeg audio directly
		if 0... 7 -> AC3 track no i, the conversion to sub stream ID / PRIVATE STREAM 1 will be done
						internally by the demuxer		                                        
                             
    begin                : Tue Oct 15 2002
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
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef __FreeBSD__
          #include <sys/types.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include "ADM_mpegindexer/ADM_mpegparser.h"
#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_dialog/DIA_working.h"

#include "ADM_audio/ADM_a52info.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_toolkit/ADM_debug.h"
//#define aprintf printf

void mpegToIndex2(char *name);
void mpegToIndex(char *name)  ;
//static void mpegPictureScan2(char *name);

uint8_t indexMpeg(char *mpeg,char *file);

static const uint32_t FPS[16]={
		0,			// 0
		23976,		// 1 (23.976 fps) - FILM
		24000,		// 2 (24.000 fps)
		25000,		// 3 (25.000 fps) - PAL
		29970,		// 4 (29.970 fps) - NTSC
		30000,		// 5 (30.000 fps)
		50000,		// 6 (50.000 fps) - PAL noninterlaced
		59940,		// 7 (59.940 fps) - NTSC noninterlaced
		60000,		// 8 (60.000 fps)
		0,			// 9
		0,			// 10
		0,			// 11
		0,			// 12
		0,			// 13
		0,			// 14
		0			// 15
	};

static uint8_t tryAudioTrack(char *name,uint8_t id,int32_t *pts);
static uint8_t getAudioTrackInfo(char *name,uint8_t id, uint32_t *chan, uint32_t *bitrate);

//static char *outname;
//static char *inname;
mParser *parser;
uint32_t nb_pic=0,skip=0,nb_aud=0;;
uint32_t sz_pic=0,sz_aud=0;

uint64_t pos_start=0;
FILE *_out;
uint8_t bfe[720*576*2];
/*
	audioTrack[24]
			00..07 -> Mpeg audio
		08+	00..07 -> AC3 audio
		16+	00..07 -> LPCM audio

*/
uint8_t  MpegaudoDetectAudio(char *name, mpegAudioTrack *audioTrack)
{

    	uint8_t streamid,audiostreamid=0x00;
	mParser *parser;
	uint32_t nbAC3=0,nbMpeg=0,nbLPCM=0;
	int32_t pts;
	
	// clear it before using it. Default = not present.
	memset(audioTrack,0,sizeof(mpegAudioTrack)*24);

	parser=new mParser();

	printf("audio Id: %x\n",audiostreamid);
	if(!parser->open(name))
		{
				GUI_Alert("Error reading mpeg !");
				delete parser;
				return 0;
		}
		uint8_t token;

	parser->sync(&token);

	// first identify stream type (ES/PS)
	delete parser;

	switch(token)
		{
				case 0xb3: // video es
									// no audio in video es
									printf("video es, no audio\n");
									return 0;
									break;
				case 0xba : // program stream ?	(vob/mpeg/VCD)
									break;
				default:
								printf("\n unrecognized stream\n");
								return 0;

		}

	printf("Program stream");
	// check for ac3 ,  i.e track 0
		
		for(uint32_t i=0;i<8;i++)
		{			
			if(tryAudioTrack(name,i,&pts))
			{
				printf("AC3 %d is present\n",i);
				audioTrack[i+8].presence=1;
				audioTrack[i+8].shift=pts;
				// Try to get info about it
				getAudioTrackInfo(name,i, &audioTrack[i+8].channels, 
								&audioTrack[i+8].bitrate);
				nbAC3++;
			}
			if(tryAudioTrack(name,0xc0+i,&pts))
			{	
				printf("Mpeg %d is present \n",i);
				audioTrack[i].presence=1;
				audioTrack[i].shift=pts;
				nbMpeg++;
			}
			if(tryAudioTrack(name,0xA0+i,&pts))
			{
				printf("LPCM %d is present \n",i);
				audioTrack[i+16].presence= 1;
				audioTrack[i+16].shift=pts;
				nbLPCM++;
			}
		}
		printf("Found %d ac3  tracks\n",nbAC3);
		printf("Found %d mpg  tracks\n",nbMpeg);
		printf("Found %d lpcm tracks\n",nbLPCM);
		if(nbAC3+nbLPCM+nbMpeg==0) 
			{
				printf("could not find audio track\n");	
				return 0;
			}
			
		return 1;
}
uint8_t tryAudioTrack(char *name,uint8_t id,int32_t *ptsShift)
{
	uint32_t count=0,audio=0;
	uint8_t streamid;
	ADM_mpegDemuxer *demuxer;

	demuxer=new ADM_mpegDemuxerProgramStream(0xe0,id);
	demuxer->open(name);

	while(demuxer->sync(&streamid) && count<1000) count++;

	audio=demuxer->getOtherSize();
	*ptsShift=demuxer->getPTSDelta();
	delete demuxer;
	printf("When trying track : %x, found : %lu bytes\n",id,audio);
	if(audio > 2000)
		{
				return 1; 
		}
	return 0;
}

uint8_t getAudioTrackInfo(char *name,uint8_t id, uint32_t *chan, uint32_t *bitrate)
{
#define AMOUNT 4*1024
	uint32_t fq=0,syncoff;
	uint8_t r=0;
	uint8_t streamid;
	uint8_t buff[AMOUNT];
	
	ADM_mpegDemuxer *demuxer;
	aprintf("Creating audio demuxer for stream %d\n",id);
	demuxer=new ADM_mpegDemuxerProgramStream(id,0xe0);
	demuxer->open(name);

	if(demuxer->read(buff,AMOUNT))	
	{			
		if( ADM_AC3GetInfo(buff, AMOUNT, &fq, bitrate,chan,&syncoff)) 
		{
			aprintf("**Track %d : Bitrate %lu\n",id,*bitrate);			
			r=1;
		}
	}
	else
	{
		printf("*cannot read track : %d\n",id);
	}
	aprintf("/Creating audio demuxer for stream %d\n",id);
	delete demuxer;
	return r;
}
	

uint8_t indexMpeg(char *mpeg,char *file,uint8_t audioid)
{
	static const char Type[5]={'X','I','P','B','D'};
	FILE *out=NULL;
	uint32_t val;
	uint32_t hnt;;
//	uint32_t in;
	uint64_t pos;
	uint64_t abspos;

	uint32_t w,h,fps ;
	uint32_t nb_iframe=0;
//	uint32_t nb_pframe=0;
//	uint32_t nb_bframe=0;
//	uint32_t first=0;
	uint32_t total_frame=0;
	uint32_t seq_found=0;
	uint64_t lastPic=0;
	uint32_t update=0;
	uint8_t  gop_seen=0;
	uint64_t lastGop=0;
	uint64_t lastAbsGop=0;
	
    uint8_t streamid,audiostreamid=0x00;
	mParser *parser;
	ADM_mpegDemuxer *demuxer;

	audiostreamid=audioid;
	parser=new mParser();

	printf("audio Id: %x\n",audiostreamid);
	if(!parser->open(mpeg))
		{
				GUI_Alert("Error reading mpeg !");
				delete parser;
				return 0;
		}
		uint8_t token;

	parser->sync(&token);

	// first identify stream type (ES/PS)
	delete parser;

	switch(token)
		{
				case 0xb3: // video es
						printf("Video Elementary stream");
						demuxer=new ADM_mpegDemuxerElementaryStream;
						break;
				case 0xba : // program stream ?	(vob/mpeg/VCD)
						printf("Program stream");

						demuxer=new  ADM_mpegDemuxerProgramStream(0xe0,audiostreamid);
						break;
				default:
						printf("\n unrecognized stream\n");
						return 0;	
		}
	// ok now we have it.
	demuxer->open(mpeg);
	out=fopen(file,"wt");
	if(!out)
	{
			printf("\n Error : cannot open index !");
			delete demuxer;
			return 0;
	}
	fprintf(out,"IDXM ");
	if(token==0xb3) fprintf(out,"E XX\n");
	else   fprintf(out,"P XX\n");
	fprintf(out,"000000000000\n");
	fprintf(out,"1\n");
	fprintf(out,"%s\n",mpeg);

	DIA_working 	*work;
	uint32_t	gop_forward=0;
	uint64_t	image_length,image_start,image_absStart;;

	work=new DIA_working("Indexing mpeg");

	while(1)
	{
				val=0;
				hnt=0;

				// sync on the video stream is done by demuxer, now get on the infos
				//
				if(!demuxer->sync(&streamid)) break;

				demuxer->getpos(&pos);
				abspos=demuxer->getAbsPos();
				update++;
			//	printf("\n sync found : %d",streamid);
				if(update>100)
					{
						if(work->update(demuxer->getAbsPos()>>16,demuxer->getSize()>>16))
						{
							// abort;
							goto stop_found;

						}
						update=0;
					}
				switch(streamid)
					{
					/* Useless apparently
						  	case 0xB9: // sequence end
                                        		printf("\n End seq\n");
                                        		goto stop_found;
                                        		break;
					*/
					case 0xB3: // sequence start
						gop_seen=1;
						// Memorize gop position
						// as it will be the real start of the
						// next I Frame
						lastGop=pos;
						lastAbsGop=abspos;
						if(seq_found)
						{
						 	demuxer->forward(8);
							break;
						}
						seq_found=1;
					        val=demuxer->read32i();
						w=val>>20;
					       	w=((w+15)&~15);
					        h= (((val>>8) & 0xfff)+15)& ~15;
					      	fps= FPS[val & 0xf];
						demuxer->forward(4);
						printf("\n %ld x %ld at %ld fps\n",w,h,fps);
						fprintf(out,"%ld %ld %ld\n",w,h,fps);

						break;
					case 0xb8: // GOP
						//	printf("GOP\n");
						uint32_t gop;	
						demuxer->forward(3);	
						gop=demuxer->read8i();
						// x Gop broken x x x x x 
						gop>>=6; // skip padding
						gop&=1;
						// take this as marker
						if(!gop_seen)
						{
							gop_seen=1;
							// Memorize gop position
							// as it will be the real start of the
							// next I Frame
							lastGop=pos;
							lastAbsGop=abspos;
						}
						// 25 bits = time code
#if 0
						if(gop)
						{
							printf("** Closed gop***\n");
							// we will change B frame to P frame
							// until we reach the next I/P
							gop_forward=2;
						}
#endif
						// 
						break;
					case 0x00 : // picture
				                //  printf("PIC\n");
						uint8_t ftype;
						uint8_t temporal_ref;
						
						total_frame++;
						val=demuxer->read16i();
						temporal_ref=val>>6;
						ftype=7 & (val>>3);
#if 0
						if(gop_forward==2 && temporal_ref!=0 && ftype==1)
						{
							printf("Fake closed gop!\n");
							gop_forward=0;
						}

						if(ftype==1 || ftype==2) // I or P
							if(gop_forward) gop_forward--;
						if(ftype==3 && gop_forward)
							ftype=2; // change B frame to P frame
#endif
						switch(ftype)
						{
						case 2:// P 
						case 3:// B
						case 4:
							// clear last memorized one
							gop_seen=0;
							if(total_frame==1)
							{
								// Skip until we met a I frame
								total_frame=0;
								continue;
							}
						case 1:// printf(" Pic : I\n");	
							//	printf("\n%c",Type[ftype]);
								
							// We conclude the previous frame
							// by indicating its length
							// that is the current post - last pic
							// Or the lastGop - last pic
							if(total_frame!=1)
							{
								if(gop_seen)
									image_length=lastGop-4-lastPic;
								else
									image_length=pos-4-lastPic;
								
								fprintf(out," %llu\n",image_length);
							}
							
							// Now set image type and start
							if(!gop_seen)
							{
								image_start=pos-4;
								image_absStart=abspos-4;
							}
							else
							{
								image_start=lastGop-4;
								image_absStart=lastAbsGop-4;
							}
							
							fprintf(out,"%c %010llX",Type[ftype],image_start);
														          							if(ftype==1) // I frame
							{
								
								fprintf(out," %010llX %10lx",
								image_absStart,demuxer->getOtherSize()); 	
							}
							if(gop_seen)
							{
								lastPic=lastGop-4;
								gop_seen=0;
							}
							else							
								lastPic=pos-4;
							nb_iframe++;
							break;

						}
						break;
					default:
					   break;
					}
		}
stop_found:
//	 	fprintf(out," %d\n\n",demuxer->getSize()-lastPic-4);
	 	fprintf(out," 0 \n\n");
       printf("\n end of stream...\n");
	  // update # of frames
	    fseek(out,0,SEEK_SET);
	  	fprintf(out,"IDXM ");
		if(token==0xb3) fprintf(out,"E %02x\n",audiostreamid);
			else   fprintf(out,"P %02x\n",audiostreamid);
		fprintf(out,"%012lX\n",nb_iframe);

		fclose(out);
		delete work;
	  printf("\n Total : %lu frames\n",total_frame);
	  if(demuxer->getPTSDelta()!=0)
	  {
	  	char string[1000];
		sprintf(string,"We have a shift of %d ms between audio and video\n",demuxer->getPTSDelta());
	  	GUI_Alert(string);
	  }
	  delete demuxer;
	  return 1;
}

