//
// C++ Implementation: ADM_lavformat
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
#include <string.h>


#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/ADM_a52info.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_editor/ADM_Video.h"

#include "ADM_lavformat.h"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_LAVFORMAT
#include "ADM_toolkit/ADM_debug.h"
#include "ADM_tscrc.h"


#define TS_NB_PACKET   1000

#define PAT_PID         0
#define PMT_PID         0x42
#define AUDIO_PID       0x45
#define VIDEO_PID       0x44

#define TS_HEADER_LEN 4

#define TS_UNIT_START        0x40
#define TS_UNIT_PAYLOAD_ONLY 0x10
#define TS_UNIT_PAYLOAD_AF   0x30
// 33 bits 
#define TS_PCR_MASK             0x0000003FFFFFFFFFLL
#define TS_NO_PCR               0xFF00000000000000LL
#define TS_PAT_EVERY_PACKET     256
#define TS_MUX_RATE             10000000        // 10 Mbs

static uint8_t writePts(uint8_t *data,uint64_t ipts);
// /* ------------------------------------------------------------------------*/write
tsMuxer::tsMuxer( void)
{
    packetPipe=new entryPacket[TS_NB_PACKET];
    memset(packetPipe,0,sizeof(entryPacket)*TS_NB_PACKET);
    packetHead=packetTail=0;
    
    audioChannel.pid=AUDIO_PID;
    audioChannel.counter=0;
    
    videoChannel.pid=VIDEO_PID;
    videoChannel.counter=0;
    
    pat.pid=PAT_PID;
    pat.counter=0;
    pat.tableId=0;
    pat.sectionId=0;
    
    pmt.pid=PMT_PID;
    pmt.counter=0;
    pmt.tableId=2;
    pmt.sectionId=1;
    
    _total=0;
    _curPTS=0;
    
    packetSincePAT=0;
    nbPacket=0;
    lastPCR=0;
    
}
tsMuxer::~tsMuxer( )
{
   if(packetPipe)
    delete [] packetPipe;
   packetPipe=NULL;

}
//
// Flush out of data packets
// update pcr on the fly with non stupid value
//
uint8_t tsMuxer::flushPackets(uint8_t really)
{
double dpcr;    // delta PCR
uint64_t pcr,packetpcr;
entryPacket *entry;
        
        dpcr=TS_PACKET_SIZE;
        dpcr/=TS_MUX_RATE;      
        dpcr=dpcr*1000*1000;        // duration of a packet in us
        pcr=(uint64_t )dpcr;
        while(1)
        {
            if(packetHead==packetTail) break; // no more packet to handle
            entry=&(packetPipe[packetHead]);
#if 0            
            if(entry->pts!=TS_NO_PCR)
            {
                if(entry->pts>=_curPTS) _curPTS=entry->pts;
                    else
                {
                    // need to update
                    uint32_t i=4;
                    uint64_t fixed;
                    uint8_t *pkt=entry->packet;
                    
                    fixed=_curPTS;
                     //mask to avoid overflow
                    fixed=(fixed*90)/1000;
                    fixed=fixed&0x1FFFFFFFFLL; // 33 Bits
                    pkt[i++]=7;
                    pkt[i++]=0x10; // AF flags       
                    pkt[i++]=(fixed>>25)&0xff; // FF
                    pkt[i++]=(fixed>>17)&0xff; // FF
                    pkt[i++]=(fixed>>9)&0xff; // FF
                    pkt[i++]=(fixed>>1)&0xff; // FF
                    pkt[i++]=(fixed<<7)&0x80; // &0x80
                    pkt[i++]=0x00;
                }
            }
#endif            
            _curPTS+=pcr;
            fwrite(entry->packet,TS_PACKET_SIZE,1,outFile);
            packetHead++;
            packetHead%=TS_NB_PACKET;
        }
        return 1;
}
/*
sync_byte 8 bslbf 
transport_error_indicator 1 bslbf       1: Error present
payload_unit_start_indicator 1 bslbf    If 1 a unit start in this packet, there is an offset later
transport_priority 1 bslbf              x
PID 13 uimsbf 
transport_scrambling_control 2 bslbf  00 not scrambled
adaptation_field_control 2 bslbf      00 01:Payload only 10:adap only 11: payload + adap
continuity_counter 4 uimsbf 
if(adaptation_field_control=='10' || adaptation_field_control=='11
'){ adaptation_field() }
if(adaptation_field_control=='01' || adaptation_field_control=='11') 
{ for (i=0;i<N;i++){ data_byte 8 bslbf } } } bslbf Bit string, 

pcr is pts in us
*/
uint8_t     tsMuxer::writePacket(uint8_t *data, uint32_t len, uint64_t pcr,channel *chan,uint8_t start)
{

uint32_t i=0,val=0;
entryPacket *paket;
uint8_t *pkt;

    if(start)
      if(((packetTail+TS_NB_PACKET-packetHead)%TS_NB_PACKET)>TS_NB_PACKET/4)
      {  
            flushPackets(0);    
      }
     
    //ADM_assert(len<TS_PACKET_SIZE-3);
    if(!packetSincePAT)
    {
        packetSincePAT++;
        writePat();
        writePmt();
    }
    else
        packetSincePAT++;
    packetSincePAT%=TS_PAT_EVERY_PACKET;
    
    //
    paket=getPacket();
    paket->pts=pcr;
    pkt=paket->packet;
    pkt[i++]=0x47;
    if(start) val=TS_UNIT_START; // Start ?
            else val=0;
    pkt[i++]=val+((chan->pid)>>8);
    pkt[i++]=(chan->pid)&0xff;
   
    if(pcr!=TS_NO_PCR)
        val=TS_UNIT_PAYLOAD_AF;     // if me put a pcr, we need to say there is
                                    // an adaption field
    else
        val=TS_UNIT_PAYLOAD_ONLY;
    pkt[i++]=val+(chan->counter&0xf);  // Payload only
    chan->counter++;
    if(pcr!=TS_NO_PCR)
    {
        
        pcr=(pcr*90)/1000;
        pcr=pcr&0x1FFFFFFFFLL; //mask to avoid overflow
        pkt[i++]=7;
        pkt[i++]=0x10; // AF flags
        
        pkt[i++]=(pcr>>25)&0xff; // FF
        pkt[i++]=(pcr>>17)&0xff; // FF
        pkt[i++]=(pcr>>9)&0xff; // FF
        pkt[i++]=(pcr>>1)&0xff; // FF
        pkt[i++]=(pcr<<7)&0x80; // &0x80
        pkt[i++]=0x00;
    }

    
    if(start) pkt[i++]=0; // pointer field
    uint32_t left=TS_PACKET_SIZE-i;
    if(len<left)
    {
        memcpy(pkt+i,data,len);
        memset(pkt+i+len,0xff,left-len);
        left=len;  
        //fwrite(pkt,TS_PACKET_SIZE,1,outFile);  
    }
    else
    {
        memcpy(pkt+i,data,left); 
        //fwrite(pkt,TS_PACKET_SIZE,1,outFile);       
    }
    
    return left;
}

uint32_t mpegTsCRC(uint8_t *data, uint32_t len)
{
    int i;
    uint32_t crc = 0xffffffff;
    
    for (i=0; i<len; i++)
        crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];
    
    return crc;
}
/*****************************/
entryPacket *tsMuxer::getPacket( void)
{
 
 entryPacket *r=&packetPipe[packetTail++];
 packetTail%=TS_NB_PACKET;
 ADM_assert(packetTail!=packetHead);
 return r;   
}

uint8_t tsMuxer::open( char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader)
{
    ADM_assert(type==MUXER_TS);
    outFile=fopen(filename,"wb");
    if(!outFile)
    {
        printf("TS:Cannot open for write\n");
        return 0; 
    }
    memcpy(&_info,info,sizeof(_info));
    memcpy(&_wavHeader,audioheader,sizeof(_wavHeader));
    
    return 1;
}
uint8_t tsMuxer::close( void)
{
    flushPackets(1);    
    if(outFile) fclose(outFile);
    outFile=NULL;
    return 1;
}
uint8_t tsMuxer::forceRestamp(void)
{
    return 1;
}
uint8_t tsMuxer::audioEmpty( void)
{
    return 0;
}
uint8_t tsMuxer::needAudio(void)
{
	
    double f;
    uint64_t pts;
    // all computation is in us
    uint32_t one=(1000*1000*1000)/_info.fps1000;
    
            pts=audioTime();
            aprintf("Need audio  ?: %llu / %llu : %llu\n ",pts,_curPTS,_curPTS+one);
            if((pts>=_curPTS) && (pts<=_curPTS+one)) return 1;
            if(pts<=_curPTS)
            {
                    printf("tsMuxer:Audio DTS is too low %llu / %llu!\n",pts,_curPTS);
                    return 1;
            }
            return 0;
}
/*
    Write the program Access Table
    In our case it is pretty simple as we have one video and one audio
    in one PMT
    We assume it fits in one TS packet

    8 :Table id
    1 :Section syntax indicator (1 for PAT)
    1: 0
    2: 11
    12: length
    16: Transport ID
    2 : 11
    5: Version number
    1: current next indicator
    8 : : section number
    8:last section number
        8: Program number
        3: 111
        13: Nit pid
    32 : CRC    
*/
uint8_t tsMuxer::writeSection( uint32_t pid,channel *chan, 
                                uint8_t *data, uint32_t len)
{
    uint8_t packet[1024];
    uint32_t i=0,sectionLen=0,crc;
    
    sectionLen=len+4+5; // Includes header & crc
    
    packet[i++]=chan->tableId;
    packet[i++]=0xb0+(sectionLen>>8);
    packet[i++]=0x00+(sectionLen&0xff);
    packet[i++]=chan->sectionId>>8;     // For PMT: Program number
    packet[i++]=chan->sectionId&0xff;
    packet[i++]=0xc1;   // Table is applicable
    packet[i++]=0;      // section  number
    packet[i++]=0;      // last section number
    
    memcpy(packet+i,data,len);
    i+=len;
    //ADM_assert(len<TS_PACKET_SIZE-3);
    // and CRC
    crc= mpegTsCRC(packet,i);
    packet[i++]=(crc>>24)&0xff;
    packet[i++]=(crc>>16)&0xff;
    packet[i++]=(crc>>8)&0xff;
    packet[i++]=(crc    )&0xff;
   
  //  writePacket(packet,i,NO_PCR);
    writePacket(packet, i, TS_NO_PCR,chan,1);
    return 1;
}
uint8_t tsMuxer::writePat( void)
{
uint8_t data[1024];
uint32_t i=0,sid=1;

        sid=pmt.sectionId;
        data[i++]=sid>>8;                
        data[i++]=sid&0xff;                        
        data[i++]=0xe0+((pmt.pid)>>8);                
        data[i++]=(pmt.pid)&0xff;    
        return writeSection(pat.pid,&pat,data,4);                    
                                
}   
uint8_t tsMuxer::writePmt( void)
{
uint8_t data[1024];
uint32_t i=0,sid=1;

        data[i++]=0xE0+(audioChannel.pid>>8);  // PCR pid locked on audio             
        data[i++]=audioChannel.pid&0xff;       //                 
        data[i++]=0xf0;  // info length              
        data[i++]=0;  // info length   
        //-- Video --
        data[i++]=0x02; // Mpeg2 video
        data[i++]=0xE0+(videoChannel.pid>>8); // pid
        data[i++]=(videoChannel.pid&0xff); 
        data[i++]=0xF0; // no descriptor
        data[i++]=0;    // no descriptor
        //-- Audio --
        if(_wavHeader.encoding==WAV_AC3)
            data[i++]=0x81; // ac3
        else
            data[i++]=0x03; // Mpeg2 audio
        data[i++]=0xE0+(audioChannel.pid>>8); // pid
        data[i++]=(audioChannel.pid&0xff); 
        data[i++]=0xF0; // no descriptor
        data[i++]=0;    // no descriptor
        
/*        data[2]=0xF0+((i-4)>>8);
        data[3]=((i-4)&0xff);
*/        
        return writeSection(pmt.pid,&pmt,data,i);                          
                                
}                                   
uint8_t tsMuxer::writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe )
{
    uint32_t part=0;
    uint8_t  data[TS_PACKET_SIZE*2];
    uint32_t pes_len;
    uint64_t its;
    double d;
   
    _curPTS=videoTime(frameno);
   
    
    while(len)
    {
        if(!part)
        {
            // we build header & pts separately
            //
            uint32_t i=0;
            //data[i++]=0x00;
            data[i++]=0x00;
            data[i++]=0x01;
            data[i++]=0xe0;   // video id
            pes_len=len+3+5+5;  // len + flags + pts  +pf 
            data[i++]=pes_len>>8;
            data[i++]=pes_len & 0xff;
            data[i++]=0x80; // mpeg2
            data[i++]=0x80+0x40; // PTS only + dts
            data[i++]=5;    // PTS LEN
            
            // pts
            its=videoTime(displayframe);
            writePts(data+i,its);
            i+=5;
            // dts
            its=videoTime(frameno);
            writePts(data+i,its);
            i+=5;
            
            uint32_t left;
            left=TS_PACKET_SIZE-i-TS_HEADER_LEN-1;  // No PCR for now
            if(left>len)
            {
                left=len;
            }
            memcpy(data+i,buf,left);
            its=videoTime(frameno);
            left=writePacket(data, i+left, its,&videoChannel,1);
            left-=i;
            // Write packet
            len-=left;
            buf+=left;
        }
        else
        {
            uint32_t left;
            
                left=writePacket(buf, len, TS_NO_PCR,&videoChannel,0);
                ADM_assert(left<=len);
                len-=left;
                buf+=left;
        }
        part++;
    }
    return 1;
}
/* Return time elapsed for video in us */
uint64_t tsMuxer::videoTime( uint32_t frameno)
{
double d;
uint64_t p;
    d=frameno;		// dts
    d=(d*1000*1000*1000);
    d=d/_info.fps1000; // in usec
    
    p= (uint64_t)floor(d);
   
    return p;
}    
/* Return time elapsed for audio in us */
uint64_t tsMuxer::audioTime( void)
{
double f;
uint64_t d;
    f=_total;
     f*=1000.*1000.;  // in us
     f/=(_wavHeader.byterate);   
     d= (uint64_t)floor(f);
     
     return d;
}
uint8_t tsMuxer::writeAudioPacket(uint32_t len, uint8_t *buf)
{
    uint32_t part=0;
    uint8_t  data[TS_PACKET_SIZE*2];
    uint32_t pes_len;
    double f;
    uint64_t ipts;

  
     ipts=audioTime(); // Time in us     
     _total+=len;
     
    while(len)
    {
        if(!part)
        {
            // we build header & pts separately
            //
            uint32_t i=0;
            //data[i++]=0x00;
            data[i++]=0x00;
            data[i++]=0x01;
            if(_wavHeader.encoding==WAV_AC3)
            {
                data[i++]=0xbd;   // audio id
            }
            else
                data[i++]=0xc0;   // audio id
            pes_len=len+3+5;  // len + flags + pts  +pf 
            data[i++]=pes_len>>8;
            data[i++]=pes_len & 0xff;
            data[i++]=0x80; // mpeg2
            data[i++]=0x80; // PTS only + dts
            data[i++]=5;    // PTS LEN
            
            // pts
            writePts(data+i,ipts);
            i+=5;
            
            
            uint32_t left;
            left=TS_PACKET_SIZE-i-TS_HEADER_LEN-1;  // No PCR for now
            if(left>len)
            {
                left=len;
            }
            memcpy(data+i,buf,left);
            left=writePacket(data, i+left, ipts,&audioChannel,1);
            left-=i;
            // Write packet
            len-=left;
            buf+=left;
            
            
        }
        else
        {
            uint32_t left;
            
                left=writePacket(buf, len, TS_NO_PCR,&audioChannel,0);
                ADM_assert(left<=len);
                len-=left;
                buf+=left;
        }
        part++;
    }
    return 1;
}
/*
    ipts is is us
*/
uint8_t writePts(uint8_t *data,uint64_t ipts)
{
uint32_t i=0;
uint64_t p;
            ipts=ipts&0xfffffffffLL; // clip to 36 bits 33 bits *8 before clipping
            ipts=(ipts*90)/1000;
            
            data[i++]=(0x02 << 4) | (((ipts >> 30) & 0x07) << 1) | 1;
            p=(((ipts >> 15) & 0x7fff) << 1) | 1;
            data[i++]=p>>8;
            data[i++]=p&0xff;
            p=(((ipts) & 0x7fff) << 1) | 1;
            data[i++]=p>>8;
            data[i++]=p&0xff;
}
// EOF
