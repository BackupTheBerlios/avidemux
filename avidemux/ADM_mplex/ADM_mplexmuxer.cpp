//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
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

#ifdef CYG_MANGLING
#define WIN32_CLASH
#define WAIT1() ADM_usleep(1000) // Allow slave thread to start
#else
#define WAIT1() sleep(1)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

#undef malloc
#undef realloc
#undef free
#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/ADM_a52info.h"

//#include "ADM_codecs/ADM_codec.h"
#include "ADM_library/avifmt.h"
#include "ADM_library/avifmt2.h"
#include "ADM_editor/ADM_Video.h"

#include "ADM_lavformat/ADM_lavformat.h"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_LAVFORMAT
#include "ADM_toolkit/ADM_debug.h"

#include "cpu_accel.h"
#include "mjpeg_types.h"
#include "mjpeg_logging.h"
#include "mpegconsts.h"

#include "interact.hpp"
#include "bits.hpp"
#include "outputstrm.hpp"
#include "multiplexor.hpp"

#include "ADM_inout.h"
#include "multiplexor.hpp"

/* 
        Define class to handle output stream(s)
      
*/


//#define VERBOSE_GOP


/***************************************************************************/


static Transfert        *channelaudio=NULL;
static Transfert        *channelvideo=NULL;
static FileOutputStream *outputStream=NULL;
static IFileBitStream   *audioin=NULL;
static IFileBitStream   *videoin=NULL;
static uint32_t         fps1000;
static int              mux_format;
static int              slaveRunning=0;

static  vector<IBitStream *> inputs;

static int slaveThread( void );

typedef  void * (*THRINP)(void *p);
//___________________________________________________________________________
mplexMuxer::mplexMuxer( void )
{
        _running=0;
        _restamp=0;

}
//___________________________________________________________________________
mplexMuxer::~mplexMuxer()
{
        close();
}
//___________________________________________________________________________
uint8_t mplexMuxer::open( char *filename, uint32_t inbitrate,ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader)
{
        printf("Opening mplex muxer (%s)\n",filename);
        _running=1;
     
        channelaudio=new Transfert();
        channelvideo=new Transfert();
        
        outputStream=new FileOutputStream ( filename );
        
        fps1000=info->fps1000;
        
        switch(type)
        {
                case MUXER_DVD: mux_format=MPEG_FORMAT_DVD_NAV;break; //FIXME
                case MUXER_VCD: mux_format=MPEG_FORMAT_VCD;break;
                case MUXER_SVCD:mux_format=MPEG_FORMAT_SVCD;break;
                default:
                        printf("Unknown muxing type\n");
                        ADM_assert(0);
        }
        printf("mplex type is :%d\n",mux_format);
       
       
        
        printf("creating slave thread\n");
        
        pthread_t slave;
        slaveRunning=1;
        ADM_assert(!pthread_create(&slave,NULL,(THRINP)slaveThread,NULL));

        WAIT1();        
        
        printf("Init ok\n");
        return 1;
}
int slaveThread( void )
{
        MultiplexJob job;
        printf("Slave thread : creating job & muxer\n");

        
        printf("output file created\n");
        
        audioin=new IFileBitStream(channelaudio);
        
        printf("audio done, creating video bitstream\n");
        videoin=new IFileBitStream(channelvideo);
        
        printf("Both stream ready\n");
         
        inputs.push_back( videoin );
        inputs.push_back( audioin );
        
        job.mux_format=mux_format;
        job.SetupInputStreams( inputs );
        

        
        Multiplexor mux(job, *outputStream);
               
        printf("Slave :Muxing\n");
        mux.Multiplex();
        slaveRunning=0;
        printf("Slace Thread exiting\n");
        pthread_exit(0);
}        
//___________________________________________________________________________
uint8_t mplexMuxer::writeAudioPacket(uint32_t len, uint8_t *buf)
{
        
        return channelaudio->write(buf,len);
}
//___________________________________________________________________________
uint8_t mplexMuxer::needAudio( void )
{
        
       return channelaudio->needData();
}
static uint8_t seq_start_code [] = {0x00, 0x00, 0x01, 0xB3};
static uint8_t gop_start_code [] = {0x00, 0x00, 0x01, 0xB8};

//___________________________________________________________________________
uint8_t mplexMuxer::writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe)
{
uint8_t r=0;   
uint16_t a1,a2,a3,a4,ff;           
        if(_restamp) // restamp timecode ?
        {
           if ( !memcmp(buf, seq_start_code, 4) || !memcmp(buf, gop_start_code, 4) )
                {
                        uint8_t *ptr;
                        uint32_t size;
                        
                        ptr=buf;
                        size=len;
                        // There is a gop a or seq header
                        if(buf[3]==0xb3) // Seq
                        {
                                while((ptr[0] || ptr[1] || ptr[2]!=1 || ptr[3]!=0xB8 ) && size>0)
                                {
                                        ptr++;
                                        size--;
                                }
                        
                        }
                        if(!size || size < 8) 
                        {
                                printf("Mplex:Gop/seq inconsistency\n");
                        }
                        else
                        {       // Now we are at gop start with a packet size
                                // Compute the current gop timestamp
                                double newtime=displayframe;
                                uint32_t hh,mm,ss,ms;
                                
                                ptr+=4; // skip gop header go to timestamp
                                
#ifdef VERBOSE_GOP              
                               
                                                       
                                a1=*ptr;
                                a2=*(ptr+1);
                                a3=*(ptr+2);
                                a4=*(ptr+3);
                                hh=(a1>>2)&0x1f;
                                mm=((a1&3)<<4)+(a2>>4);
                                ss=((a2&7)<<3)+(a3>>5);
                                ff=((a3&0x1f)<<1)+(a4>>7);

                                printf("Old : h:%02d m:%02d s:%02d f:%02d\n",hh,mm,ss,ff);
#endif  
                                
                                newtime=(newtime*1000);
                                newtime/=fps1000; // in seconds
                                
                                hh=(uint32_t)newtime/3600;
                                newtime-=hh*3600;
                                mm=(uint32_t)newtime/60;
                                newtime-=mm*60;
                                ss=(uint32_t)newtime;
                                newtime-=ss;
                                newtime*=1000;
                                ms=(uint32_t)newtime;
                                
                                *(ptr+0)=(hh<<2)+(mm>>4);
                                *(ptr+1)=((mm&0xf)<<4)+8+(ss>>3);
                                *(ptr+2)= ((ss&7)<<5)+(ff>>1);
                                *(ptr+3)&=0x7f;
                                *(ptr+3)+=(ms&1) <<7;
                                
#ifdef VERBOSE_GOP              
                               
                                                       
                                a1=*ptr;
                                a2=*(ptr+1);
                                a3=*(ptr+2);
                                a4=*(ptr+3);
                                hh=(a1>>2)&0x1f;
                                mm=((a1&3)<<4)+(a2>>4);
                                ss=((a2&7)<<3)+(a3>>5);
                                ff=((a3&0x1f)<<1)+(a4>>7);

                                printf("New : h:%02d m:%02d s:%02d f:%02d\n",hh,mm,ss,ff);
#endif  
                                
                        
                        }
        
                }
        }
        r= channelvideo->write(buf,len);
        while(channelvideo->fillingUp())
        {
                printf("Output buffer filling up, sleeping a bit\n");
                WAIT1();
        }                
        
        return 1;
}
//___________________________________________________________________________
uint8_t mplexMuxer::forceRestamp(void)
{
        _restamp=1;
        return 1;
}
//___________________________________________________________________________
uint8_t mplexMuxer::close( void )
{
        if(_running)
        {
                _running=0;
                channelvideo->abort();
                channelaudio->abort();
                while(slaveRunning)
                {
                        printf("Waiting for slave thread to end\n");
                        WAIT1();
                }
                        // Flush
                        // Cause deadlock :
                delete audioin;
                delete videoin;
                delete channelvideo;
                delete channelaudio;
                delete outputStream;
                inputs.erase( inputs.begin(), inputs.end() );
                printf("Mplex : All destroyed\n");
        }
        return 1;
}
//___________________________________________________________________________
uint8_t mplexMuxer::audioEmpty( void)
{
        return 0;
}


//EOF
