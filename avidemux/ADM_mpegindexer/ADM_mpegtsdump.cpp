/***************************************************************************
  Small scanner for mpeg TS stream
    
            
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"
#include <ADM_assert.h>
#include "ADM_library/fourcc.h"
#include "ADM_mpegindexer/ADM_mpegparser.h"
#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_TS.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_dialog/DIA_working.h"

#include "ADM_audio/ADM_a52info.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_toolkit/ADM_debug.h"
//___________________________
#include "ADM_mpegindexer/ADM_mpegTs.h"
static uint8_t tryAudioTrack(char *name,uint8_t id,mpegAudioTrack *track);
static uint8_t getAC3AudioTrackInfo(char *name,uint32_t id, uint32_t *chan, uint32_t *bitrate,uint8_t tsstream);
static uint8_t getMP2AudioTrackInfo(char *name,uint32_t id, uint32_t *chan, uint32_t *bitrate,uint8_t tsstream);
//___________________________
int sync( FILE *fd);
int parse(FILE *fd);
int usage(void );
int push(int pid, int streamid,uint64_t pts);
//___________________________
static ADM_TsTrackInfo stack[100];
static uint32_t stacked=0;
static uint8_t buffer[TS_PACKET*4];
//___________________________
uint8_t  MpegaudoDetectAudio(char *name, mpegAudioTrack *audioTrack)
{

    	uint8_t streamid,audiostreamid=0x00;
	mParser *parser;
	uint32_t nbAC3=0,nbMpeg=0,nbLPCM=0;
	int32_t pts;
	uint32_t vid=0;
	uint8_t token;
	// clear it before using it. Default = not present.
	memset(audioTrack,0,sizeof(mpegAudioTrack)*24);
	printf("audio Id: %x\n",audiostreamid);
	
	parser=new mParser();

	
	if(!parser->open(name))
		{
				GUI_Alert("Error reading mpeg !");
				delete parser;
				return 0;
		}
	if(parser->read8i()==0x47) // Mpeg TS
	{
		uint32_t nb;
		ADM_TsTrackInfo *info;
		
		if(! ADM_TsParse(name,&nb,&info)) return 0;
		
		uint32_t nbvid=0;
		uint32_t id;
		
		printf("Found %d tracks\n",nb);
		for(uint32_t i=0;i<nb;i++)
		{
			id=info[i].es;
			printf("Id:%x\n",id);
			if(id>=0xc0 && id<=0xcf) // Mpeg1/2 audio
			{
				audioTrack[nbMpeg].presence=1;
				audioTrack[nbMpeg].shift=i;
				nbMpeg++;				
			}
			else
			if(id==0xbd) // private stream
			{
				audioTrack[nbAC3+8].presence=1;
				audioTrack[nbAC3+8].shift=i;
				nbAC3++;
			}
			else if(id>=0xE0 && id<=0xEF)
			{
				if(!nbvid) vid=i;
				nbvid++;
			}
		}
		if(!nbvid)
		{
			GUI_Alert("No video track found!");
			return 0;
		}
		printf("Video is pid :%x\n",vid);
		if((nbAC3+nbMpeg)==0)
		{
			GUI_Alert("No audio track found\n");
			return 0;	
		}
		// Now that we have the video stream
		// we can compute the a/v sync value
		nbAC3=nbMpeg=0;
		int64_t vidPts=info[vid].pts;
		
		int64_t audPts=0;
		uint32_t track;
		double d;
		
		printf("Vid is tack %d, with pts %lu\n",vid,info[vid].pts);
		for(uint32_t i=0;i<8;i++)
		{
			//printf("%d<\n",i);			
			if(audioTrack[i].presence)
			{
				track=audioTrack[i].shift;
				audPts=info[track].pts;
				printf("MPEG aud %lu %d\n",audPts,audioTrack[i].shift);
				d=vidPts;
				d-=audPts;
				d/=90.;
				audioTrack[i].shift=(int32_t)floor(d);
				printf("%ld ms\n",(int32_t)floor(d));
				getMP2AudioTrackInfo(name,info[track].pid, &audioTrack[i].channels, 
								&audioTrack[i].bitrate,1);
				
			}
			if(audioTrack[i+8].presence)
			{
				
				track=audioTrack[i+8].shift;
				audPts=info[track].pts;
				printf("AC3 aud %li %d\n",audPts,audioTrack[i+8].shift);
				d=vidPts;
				d-=audPts;
				d/=90.;
				audioTrack[i+8].shift=(int32_t)floor(d);
				printf("%ld ms\n",(int32_t)floor(d));
				getAC3AudioTrackInfo(name,info[track].pid, &audioTrack[i+8].channels, 
								&audioTrack[i+8].bitrate,1);
				
			}
			
		}
		
		return 1;
		
		
	}
	else
	{
	parser->setpos(0);
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
	// First we do a quick pick to find what is present in the stream
	//
	ADM_mpegDemuxerProgramStream *parser;
	uint8_t id;
	
	parser=new ADM_mpegDemuxerProgramStream(0xE0,0xFF);
	if(!parser->open(name)) 
	{
		delete parser;
		return 0;
	}
	
	uint32_t count=5000; // =~ 10 Mbytes scanned
	while(count>0)
	{
		if(!parser->peekPacket(&id)) break;
		if(id>=0xC0 && id<=0xc7) 
		{
			audioTrack[id-0xC0].presence=1;
		}
		if(id>=0xA0 && id<=0xA7) 
		{
			audioTrack[id-0xA0+16].presence=1;
		}
		if(id<=0x7)
		{
		 audioTrack[id+8].presence=1;
		}
		count--;
	}
	delete parser;
	// Then we do get more info on the present stream(s)
	for(uint32_t i=0;i<8;i++)
		{		
			if(audioTrack[i+8].presence)	
			if(tryAudioTrack(name,i,&audioTrack[i+8]))
			{
				printf("AC3 %d is present\n",i);				
				nbAC3++;
			}
			if(audioTrack[i].presence)
			if(tryAudioTrack(name,0xc0+i,&audioTrack[i]))
			{	
				printf("Mpeg %d is present \n",i,&audioTrack[i]);
				nbMpeg++;
			}
			if(audioTrack[i+16].presence)
			if(tryAudioTrack(name,0xA0+i,&audioTrack[i+16]))
			{
				printf("LPCM %d is present \n",i);
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

	}	
	return 1;
}
//
//
//______________________________________________________
uint8_t tryAudioTrack(char *name,uint8_t id,mpegAudioTrack *track)
{
int32_t ptsShift=0;

	uint32_t count=0,audio=0;
	uint8_t streamid;
	ADM_mpegDemuxer *demuxer;

	// If packets have been detected assume lpcm is there
	if(id>=0xA0 && id<=0xA7)
	{
		track->channels=2;
		track->bitrate=1536;
		return 1;
	}
	
	demuxer=new ADM_mpegDemuxerProgramStream(0xe0,id);
	demuxer->open(name);

	while(demuxer->sync(&streamid) && count<1000) count++;

	audio=demuxer->getOtherSize();
	ptsShift=demuxer->getPTSDelta();
	delete demuxer;
	printf("When trying track : %x, found : %lu bytes\n",id,audio);
	
	if(audio > 2000)
		{
				// now read it
				track->shift=ptsShift;
				if(id<8)
				{
					if(getAC3AudioTrackInfo(name,id, &(track->channels)
						, &(track->bitrate),0))
						{
							return 1;
						}
				}
				else if(id<0xcf)
				{
					if(getMP2AudioTrackInfo(name,id, &(track->channels)
						, &(track->bitrate),0))
						{
							return 1;
						}
				
				}
				return 1; 
		}
	return 0;
}

//
//
//_________________________________________________________
uint8_t getAC3AudioTrackInfo(char *name,uint32_t id, uint32_t *chan, uint32_t *bitrate,uint8_t tsstream)
{
#define AMOUNT 5*1024
	uint32_t fq=0,syncoff;
	uint8_t r=0;
	uint8_t streamid;
	uint8_t buff[AMOUNT];
	
	ADM_mpegDemuxer *demuxer;
	aprintf("Creating audio demuxer for stream %d\n",id);
	if(tsstream)
		demuxer=new ADM_mpegDemuxerTransportStream(id,0xff);
	else
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
uint8_t getMP2AudioTrackInfo(char *name,uint32_t id, uint32_t *chan, uint32_t *bitrate,uint8_t tsstream)
{
#define AMOUNT 5*1024
	uint32_t fq=0,syncoff;
	uint8_t r=0;
	uint8_t streamid;
	uint8_t buff[AMOUNT];
	WAVHeader wav;
	
	ADM_mpegDemuxer *demuxer;
	aprintf("Creating audio demuxer for stream %d\n",id);
	if(tsstream)
		demuxer=new ADM_mpegDemuxerTransportStream(id,0xff);
	else
		demuxer=new ADM_mpegDemuxerProgramStream(id,0xe0);
	demuxer->open(name);

	if(demuxer->read(buff,AMOUNT))	
	{		
		if( mpegAudioIdentify(buff, AMOUNT, &wav,NULL))
		
		{
			*bitrate=wav.byterate;
			*chan=wav.channels;
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
//
// The 
//__________________________________
uint8_t ADM_TsParse(char *file,uint32_t *nbTrack, ADM_TsTrackInfo **token)
{
FILE *fd;
	printf("Scanning mpeg Ts\n");
	fd=fopen(file,"rb");
	stacked=0;
	if(!fd)
	{
		
			printf("Cannot open %s\n",file);
			return 0;
	}
	if(!sync(fd)) { printf("Cannot find sync\n");return 0;}
	parse(fd);
	*nbTrack=stacked;
	*token=stack;
	printf("\nDone.\n");
}

int sync( FILE *fd)
{
int n=0;

	fread(buffer,TS_PACKET,3,fd);
	while(n<TS_PACKET*2)
	{
		if(buffer[n]==0x47 && buffer[n+TS_PACKET]==0x47)
		{
			fseek(fd,n,SEEK_SET);
			return 1;
		}
		n++;
	}
	return 0;
}
// Parse until we find a PES header to identify the stream
//
int parse(FILE *fd)
{
int loop=0;
int pid,adapt,start,len;
int esid;
uint64_t pts=0;
	while(loop<10000)
	{
		pts=0;
		loop++;		
		if(1!=fread(buffer,TS_PACKET,1,fd))
		{
			printf("Read error\n");
			 return 0;
		}
		if(buffer[0]!=0x47)
		{
			printf("Sync lost\n");
			return 0;
		}
		pid=((buffer[1]<<8)+buffer[2]) & 0x1FFF;
		if(pid<0x10) continue; // PAT etc...
		adapt=(buffer[3]&0x30)>>4;
		if(!(adapt&1)) continue; // no payload
		
		start=4;
		len=184;
		if(adapt==3) //payload + adapt
		{			
			start+=1+buffer[4];
			len-=1+buffer[4];
		}
		// We got a packet of the correct pid
		if(!(buffer[1]&0x40)) // PES header ?
			continue;
		if(buffer[start] || buffer[start+1] || buffer[start+2]!=01)
		{
			printf("Weird...%x :%x %x %X\n",pid,buffer[start],buffer[start+1],buffer[start+2]);
			continue;		
		}
		// Next 
		esid=buffer[start+3];
		start+=4;
		// look if pts is present...
		
		// 2: Size of PES
		// 1: Flags scrambling align etc...
		// 1: PTS DTS
		// {1] addition header len
		// [5] PTS[5] DTS ...
		start+=3;
		if((buffer[start]>>7) & 1)
		{
			start+=2;
			pts=(((buffer[start]&6)>>1)<<30)+
				(buffer[start+1]<<22)
				+((buffer[start+2]>>1)<<15)
				+(buffer[start+3]<<7)
				+(buffer[start+4]>>1);
		}
		push(pid,esid,pts);
	}	
	return 1;
}
//
//	Put in stack the triplet es id, packet id and pts 
//
int push(int pid, int streamid,uint64_t pts)
{
uint32_t key;
	key=(pid<<16)+streamid;
	for(int i=0;i<stacked;i++)
	{
		if((stack[i].es==streamid) && (stack[i].pid==pid))
		{
			// already in there ...
			// if old pts=0 update with current one
			if(!stack[i].pts) stack[i].pts=pts;
		 	return 1;
		}
	}
	stack[stacked].es=streamid;
	stack[stacked].pid=pid;
	stack[stacked].pts=pts;
	stacked++;
	printf("Found pid: %x with stream : %x pts:%llx\n",pid,streamid,pts);
	return 1;

}
