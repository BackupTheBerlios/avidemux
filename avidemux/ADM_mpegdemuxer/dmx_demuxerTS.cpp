/***************************************************************************
                          TS demuxer
                             -------------------
                
    copyright            : (C) 2005 by mean
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
#include <ADM_assert.h>



#include "dmx_demuxerTS.h"
uint8_t         dmx_demuxerTS::changePid(uint8_t newpid)
{
          myPid=newpid;
          
        _pesBufferStart=0;  // Big value so that we read
        _pesBufferLen=0;
        _pesBufferIndex=0;
}
dmx_demuxerTS::dmx_demuxerTS(uint32_t nb,MPEG_TRACK *tracks)
{
        consumed=0;
        parser=new fileParser();
        stampAbs=0;
        _pesBuffer=new uint8_t [MAX_TS_BUFFER];

        memset(seen,0,sizeof(seen));

        _pesBufferStart=0;  // Big value so that we read
        _pesBufferLen=0;
        _pesBufferIndex=0;
        ADM_assert(nb>0);
        tracked=NULL;

        nbTracked=nb;
        
        myPid=tracks[0].pid; // For mpeg TS we use the PID field as the PES field is irrelevant
        
        if(nb!=256)     // Only pick one track as main, and a few as informative
        {
#if 0                
                memset(mask,0,256);
                tracked=new uint8_t[nbTracked];
                for(int i=1;i<nb;i++)
                {
                        mask[i]=1;
                        tracked[i]=tracks[i].pes&0xff;
                }                
#endif
                
        }else
        {
#if 0
                memset(mask,1,256); // take all tracks
#endif
        }


       	_probeSize=0; 
       	memset(seen,0,255*sizeof(uint64_t));     
        printf("Creating mpeg PS demuxer  main Pid: %X \n",myPid);
}
dmx_demuxerTS::~dmx_demuxerTS()
{
        if(parser) delete parser;
        parser=NULL;
        if(_pesBuffer) delete [] _pesBuffer;
        _pesBuffer=NULL;
        if(tracked) delete [] tracked;
        tracked=NULL;
}
/*
        Get stats about the PES ids tracked in tracked order
        if nbTracked=256 it means tracks all possible PES id
        The 0 rank is video
*/
uint8_t       dmx_demuxerTS::getStats(uint64_t *oseen)
{
        if(nbTracked!=256)
        {
                oseen[0]=0;
                for(int i=1;i<nbTracked;i++)
                {
                        oseen[i]=seen[tracked[i]];
                }
        }
        else
        {
                 for(int i=0;i<nbTracked;i++)
                {
                        oseen[i]=seen[i];
                }
        }
        return 1;
}
uint8_t dmx_demuxerTS::setProbeSize(uint32_t sz)
{
		_probeSize=sz;
		return 1;
}
uint8_t dmx_demuxerTS::open(char *name)
{
        if(! parser->open(name)) return 0;
        _size=parser->getSize();
        
        return 1;
}
uint8_t dmx_demuxerTS::forward(uint32_t f)
{
uint32_t left;        
        if(_pesBufferIndex+f<=_pesBufferLen) 
        {
                _pesBufferIndex+=f;
                consumed+=f;
                return 1;
        }
        // else read another packet
        left=_pesBufferLen-_pesBufferIndex;
        f-=left;
        consumed+=left;
        if(!refill()) return 0;
        return forward(f);
}
uint8_t  dmx_demuxerTS::stamp(void)
{
        consumed=0;        
}
uint64_t dmx_demuxerTS::elapsed(void)
{
        return consumed;        
}
uint8_t  dmx_demuxerTS::getPos( uint64_t *abs,uint64_t *rel)
{
        *rel=_pesBufferIndex;
        *abs=_pesBufferStart;       
        return 1;
}
uint8_t dmx_demuxerTS::setPos( uint64_t abs,uint64_t  rel)
{
				// Need to move ?
				if(abs==_pesBufferStart && _pesBufferLen)
				{
						if(_pesBufferLen<rel)
							{
								printf("Asked setpos to go %lu whereas %lu is max\n",
											rel,_pesBufferLen);
								ADM_assert(rel<_pesBufferLen);
							}
						
					  _pesBufferIndex=rel;
					  return 1;
					
				}
               if(!parser->setpos(abs))
                {
                        printf("DMX_PS: setPos failed\n");
                         return 0;
                }
                _pesBufferStart=abs;
                if(!refill())
                {
                        printf("DMX_PS: refill failed\n");
                        return 0;
                }
                
                if(rel>_pesBufferLen)
                {
                        printf("Set pos failed : asked rel:%lu max: %lu, absPos:%llu absPosafterRefill:%llu\n",
                                        rel,_pesBufferLen,abs,_pesBufferStart);
                        ADM_assert(rel<_pesBufferLen);                        
                }

                _pesBufferIndex=rel;
                return 1;
               
}
/*
        Sync on mpeg sync word, returns the sync point in abs/r
*/



uint32_t         dmx_demuxerTS::read(uint8_t *w,uint32_t len)
{
uint32_t mx;
                // enough in buffer ?
                if(_pesBufferIndex+len<=_pesBufferLen)
                {
                        memcpy(w,_pesBuffer+_pesBufferIndex,len);
                        _pesBufferIndex+=len;
                        consumed+=len;
                        return len;
                }
                // flush
                mx=_pesBufferLen-_pesBufferIndex;
                if(mx)
                {
                        memcpy(w,_pesBuffer+_pesBufferIndex,mx);
                        _pesBufferIndex+=mx;
                        consumed+=mx;
                        w+=mx;
                        len-=mx;
                }
                if(!refill())
                {
                        _lastErr=1;
                         return 0;
                }
                return mx+read(w,len);
}
uint8_t         dmx_demuxerTS::sync( uint8_t *stream,uint64_t *abs,uint64_t *r,uint64_t *pts,uint64_t *dts)
{
uint32_t val,hnt;
         *r=0;

                val=0;
                hnt=0;

                // preload
                hnt=(read8i()<<16) + (read8i()<<8) +read8i();
                if(_lastErr)
                {
                        _lastErr=0;
                        printf("\n io error , aborting sync\n");
                        return 0;       
                }

                while((hnt!=0x00001))
                {

                        hnt<<=8;
                        val=read8i();
                        hnt+=val;
                        hnt&=0xffffff;
 
                        if(_lastErr)
                        {
                             _lastErr=0;
                            printf("\n io error , aborting sync\n");
                            return 0;
                         }

                }

                *stream=read8i();
                // Case 1 : assume we are still in the same packet
                if(_pesBufferIndex>=4)
                {
                        *abs=_pesBufferStart;
                        *r=_pesBufferIndex-4;
                        *pts=_pesPTS;
                        *dts=_pesDTS;
                }
                else
                {       // pick what is needed from oldPesStart etc...
                        // since the beginning in the previous packet
                        uint32_t left=4-_pesBufferIndex;
                                 left=_oldPesLen-left;
#if 0
                                 printf("Next packet : %I64X Len :%lu, using previous packet %I64X len:%u as pos=%lu\n",
                                 		_pesBufferStart,_pesBufferLen,_oldPesStart,_oldPesLen,_pesBufferIndex);
#endif
                                 if(left>_oldPesLen)
                                {
                                        printf("Need %lu bytes from previous packet, which len is %lu\n",left,_oldPesLen);
                                        ADM_assert(0);
                                }
                                *abs=_oldPesStart;
                                *r=left;
                                *pts=_oldPTS;
                                *dts=_oldDTS;
                }
                return 1;
}

//
//      Refill the pesBuffer
//              Only with the pid that is of interest for us
//              Update PTS/DTS
//              Keep track of other pes len
//
uint8_t dmx_demuxerTS::refill(void)
{
uint32_t globstream,len,pid,payloadunit=0;
uint8_t  stream,substream;
uint64_t count,abs,pts,dts;
uint8_t  head1,head2;
uint8_t  tmp[TS_PACKET_SIZE];
        // Resync on our stream
_again:
        
        parser->getpos(&count);
        count=_size-count;
        while(parser->read8i()!=0x47 && abs>TS_PACKET_SIZE) abs--;
        if(count<TS_PACKET_SIZE)
        {
                printf("DmxTS: cannot sync  \n");
                _lastErr=1;
                return 0;
        }
        // Read Pid etc...
        pid=parser->read16i();
        if(pid & TS_UNIT_START) payloadunit=1;
        pid&=0x1fff; // remove flags
        // Read leftover
        parser->read32(TS_PACKET_SIZE-3,tmp);
        parser->getpos(&abs);
        abs-=3;
#if 0
        if(parser->peek8i()!=0x47)
        {
                
                printf("DmxTs: broken packet at %"LLX"\n",abs);
                goto _again;
        }
#endif        
        if(_probeSize)
        {
                if(abs>_probeSize)
                {
                		printf("dmxTs:Probe exceeded\n");
                		return 0;
                }
        }
        // Ok now get some informations....
        // only interested in my Pid & user data Pid
        if(pid!=myPid && pid<0x20)
        {
                goto _again;
        }
        // One of the stream we are looking for ?
        if(!allPid[pid]) goto _again; // No
        // Remove header if any
        int index=0;
        int cc,val,adaptation;
        val=tmp[index++];
        cc=val & 0xf;
        adaptation=(val >>4)&0x3;
        if(adaptation & 2) // There is an adaptation field
        {
                val=tmp[index++];
                index+=val; // skip adaptation field
        }
        // Ok now we got the raw data packet
        if(payloadunit) // A PSI or PES packet starts here
        {
                // There is a PES / PSI header
                // PES...
                int left=TS_PACKET_SIZE-3-index;
                

        }
        else
        {
                if(pid==myPid)
                {
                        _pesDTS=dts;
                        _pesPTS=pts;
                        _pesBufferStart=abs;
                        _pesBufferLen=TS_PACKET_SIZE-3-index;;
                        _pesBufferIndex=0;
                        memcpy(_pesBuffer,&tmp[index],_pesBufferLen);
                        return 1;
                }
        }
        goto _again;
}
/*
        Retrieve info about the packet we just met
        It is assumed that parser is just after the packet startcode

*/
uint8_t dmx_demuxerTS::getPacketInfo(uint8_t stream,uint8_t *substream,uint32_t *olen,uint64_t *opts,uint64_t *odts)
{

//uint32_t un ,deux;
uint64_t size=0;
uint8_t c,d;
uint8_t align=0;
                        
                *substream=0xff;
                *opts=ADM_NO_PTS;
                *odts=ADM_NO_PTS;
                
#if 0
                size=parser->read16i();
                if((stream==PADDING_CODE) || 
                	 (stream==PRIVATE_STREAM_2)
                        ||(stream==SYSTEM_START_CODE) //?
                        ) // special case, no header
                        {
                                *olen=size;      
                                return 1;
                        }
#endif
                        //      remove padding if any                                           
        
                while((c=parser->read8i()) == 0xff) 
                {
                        size--;
                }
//----------------------------------------------------------------------------
//-------------------------------MPEG-2 PES packet style----------------------
//----------------------------------------------------------------------------
                if(((c&0xC0)==0x80))
                {
                        uint32_t ptsdts,len;
                        //printf("\n mpeg2 type \n");
                        //_muxTypeMpeg2=1;
                        // c= copyright and stuff       
                        //printf(" %x align\n",c);      
                        if(c & 4) align=1;      
                        c=parser->read8i();     // PTS/DTS
                        //printf("%x ptsdts\n",c
                        ptsdts=c>>6;
                        // header len
                        len=parser->read8i();
                        size-=3;  

                        switch(ptsdts)
                        {
                                case 2: // PTS=1 DTS=0
                                        if(len>=5)
                                        {
                                                uint64_t pts1,pts2,pts0;
                                                //      printf("\n PTS10\n");
                                                        pts0=parser->read8i();  
                                                        pts1=parser->read16i(); 
                                                        pts2=parser->read16i();                 
                                                        len-=5;
                                                        size-=5;
                                                        *opts=(pts1>>1)<<15;
                                                        *opts+=pts2>>1;
                                                        *opts+=(((pts0&6)>>1)<<30);
                                        }
                                        break;
                                case 3: // PTS=1 DTS=1
                                                #define PTS11_ADV 10 // nut monkey
                                                if(len>=PTS11_ADV)
                                                {
                                                        uint32_t skip=PTS11_ADV;
                                                        uint64_t pts1,pts2,dts,pts0;
                                                                //      printf("\n PTS10\n");
                                                                pts0=parser->read8i();  
                                                                pts1=parser->read16i(); 
                                                                pts2=parser->read16i(); 
                                                                                        
                                                                *opts=(pts1>>1)<<15;
                                                                *opts+=pts2>>1;
                                                                *opts+=(((pts0&6)>>1)<<30);
                                                                pts0=parser->read8i();  
                                                                pts1=parser->read16i(); 
                                                                pts2=parser->read16i();                 
                                                                dts=(pts1>>1)<<15;
                                                                dts+=pts2>>1;
                                                                dts+=(((pts0&6)>>1)<<30);
                                                                len-=skip;
                                                                size-=skip;
                                                                *odts=dts;
                                                                        //printf("DTS: %lx\n",dts);                
                                                   }
                                                   break;               
                                case 1:
                                                return 0;//ADM_assert(0); // forbidden !
                                                break;
                                case 0: 
                                                // printf("\n PTS00\n");
                                                break; // no pts nor dts
                                                                                
                                                            
                        }  
// Extension bit        
// >stealthdave<                                

                        // Skip remaining headers if any
                        if(len) 
                        {
                                parser->forward(len);
                                size=size-len;
                        }

#if 0
                if(stream==PRIVATE_STREAM_1)
#endif
                {
                        if(size>5)
                        {
                        // read sub id
                               *substream=parser->read8i();
  //                    printf("\n Subid : %x",*subid);
                                switch(*substream)
                                {
                                //AC3
                                        case 0x80:case 0x81:case 0x82:case 0x83:
                                        case 0x84:case 0x85:case 0x86:case 0x87:
                                                *substream=*substream-0x80;
                                                break;
                                // PCM
                                        case 0xA0:case 0xA1:case 0xa2:case 0xa3:
                                        case 0xA4:case 0xA5:case 0xa6:case 0xa7:
                                                // we have an additionnal header
                                                // of 3 bytes
                                                parser->forward(3);
                                                size-=3;
                                                break;
                                // Subs
                                case 0x20:case 0x21:case 0x22:case 0x23:
                                case 0x24:case 0x25:case 0x26:case 0x27:
                                                break;
                             
                                default:
                                                *substream=0xff;
                                }
                                // skip audio header (if not sub)
                                if(*substream>0x26 || *substream<0x20)
                                {
                                        parser->forward(3);
                                        size-=3;
                                }
                                size--;
                        }
                }
               //    printf(" pid %x size : %x len %x\n",sid,size,len);
                *olen=size;
                return 1;
        }
//----------------------------------------------------------------------------------------------                
//-------------------------------MPEG-1 PES packet style----------------------                                  
//----------------------------------------------------------------------------------------------                                        
           if(0) //_muxTypeMpeg2)
                {
                        printf("*** packet type 1 inside type 2 ?????*****\n");
                        return 0; // mmmm                       
                }
          // now look at  STD buffer size if present
          // 01xxxxxxxxx
          if ((c>>6) == 1) 
          {       // 01
                        size-=2;
                        parser->read8i();                       // skip one byte
                        c=parser->read8i();   // then another
           }                       
           // PTS/DTS
           switch(c>>4)
           {
                case 2:
                {
                        // 0010 xxxx PTS only
                        uint64_t pts1,pts2,pts0;
                                        size -= 4;
                                        pts0=(c>>1) &7;
                                        pts1=parser->read16i()>>1;
                                        pts2=parser->read16i()>>1;
                                        *opts=pts2+(pts1<<15)+(pts0<<30);
                                        break;
                  }
                  case 3:
                  {               // 0011 xxxx
                        uint64_t pts1,pts2,pts0;
                                        size -= 9;
                                                                        
                                        pts0=(c>>1) &7;
                                        pts1=parser->read16i()>>1;
                                        pts2=parser->read16i()>>1;
                                        *opts=pts2+(pts1<<15)+(pts0<<30);
                                        parser->forward(5);
                   }                                                               
                   break;
                   
                case 1:
                        // 0001 xxx             
                        // PTSDTS=01 not allowed                        
                                return 0;
                                break; 
                }
                                                                

                if(!align)      
                        size--;         
        *olen=size;
        return 1;
}
