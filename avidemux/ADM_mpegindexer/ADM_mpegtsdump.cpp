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
#include <assert.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

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
uint8_t ADM_TsParse(char *file,uint32_t *nbTrack, uint32_t **token);
static uint8_t tryAudioTrack(char *name,uint8_t id,int32_t *pts);
static uint8_t getAudioTrackInfo(char *name,uint8_t id, uint32_t *chan, uint32_t *bitrate);

//___________________________
int sync( FILE *fd);
int parse(FILE *fd);
int usage(void );
int push(int pid, int streamid);
//___________________________
static uint32_t stack[100];
static uint32_t stacked=0;
static uint8_t buffer[TS_PACKET*4];
//___________________________
uint8_t  MpegaudoDetectAudio(char *name, mpegAudioTrack *audioTrack)
{

    	uint8_t streamid,audiostreamid=0x00;
	mParser *parser;
	uint32_t nbAC3=0,nbMpeg=0,nbLPCM=0;
	int32_t pts;
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
		uint32_t nb,*info;
		if(! ADM_TsParse(name,&nb,&info)) return 0;
		
		uint32_t nbvid=0;
		uint32_t id;
		
		printf("Found %d tracks\n",nb);
		for(uint32_t i=0;i<nb;i++)
		{
			id=info[i]&0xffff;
			printf("Id:%x\n",id);
			if(id>=0xc0 && id<=0xcf)
			{
				audioTrack[nbMpeg].presence=1;
				nbMpeg++;				
			}
			else
			if(id==0xbd) // private stream
			{
				audioTrack[nbAC3+8].presence=1;
				nbAC3++;
			}
			else if(id>=0xE0 && id<=0xEF)
			{
				nbvid++;
			}
		}
		if(!nbvid)
		{
			GUI_Alert("No video track found!");
			return 0;
		}
		if((nbAC3+nbMpeg)==0)
		{
			GUI_Alert("No audio track found\n");
			return 0;	
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
	}		
	return 1;
}
//
//
//______________________________________________________
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
//
//
//_________________________________________________________
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
	
//
//
//__________________________________
uint8_t ADM_TsParse(char *file,uint32_t *nbTrack, uint32_t **token)
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

	while(loop<10000)
	{
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
		push(pid,buffer[start+3]);
	}	
	return 1;
}
int push(int pid, int streamid)
{
uint32_t key;
	key=(pid<<16)+streamid;
	for(int i=0;i<stacked;i++)
	{
		if(stack[i]==key) return 1;
	}
	stack[stacked++]=key;
	printf("Found pid: %x with stream : %x\n",pid,streamid);
	return 1;

}
