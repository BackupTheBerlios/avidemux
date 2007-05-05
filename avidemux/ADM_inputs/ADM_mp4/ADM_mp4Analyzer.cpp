/***************************************************************************
/*
    
    copyright            : (C) 2007 by mean
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

#include "math.h"

#include <ADM_assert.h>
#include "default.h"
#include "ADM_editor/ADM_Video.h"


#include "fourcc.h"
#include "ADM_mp4/ADM_mp4.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_codecs/ADM_codec.h"

#include "ADM_mp4Tree.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_3GP
#include "ADM_osSupport/ADM_debug.h"


#define TRACK_OTHER 0
#define TRACK_AUDIO 1
#define TRACK_VIDEO 2

#define MAX_CHUNK_SIZE (3*1024)

// 14496-1 / 8.2.1
typedef enum MP4_Tag
{
	Tag_InitialObjDesc	=0x02,
	Tag_ES_Desc		=0x03,
	Tag_DecConfigDesc 	=0x04,
	Tag_DecSpecificInfo 	=0x05
};

extern char* ms2timedisplay(uint32_t ms);

/**
      \fn    LookupMainAtoms
      \brief Search main atoms to ease job for other part
*/
uint8_t     MP4Header::lookupMainAtoms(void *ztom)
{
  
  adm_atom *tom=(adm_atom *)ztom;
  adm_atom *moov;
  ADMAtoms id;
  uint32_t container;
  printf("Analyzing file and atoms\n");
  if(!ADM_mp4SimpleSearchAtom(tom, ADM_MP4_MOOV,&moov))
  {
       adm_printf(ADM_PRINT_ERROR,"Cannot locate moov atom\n");
       return 0;
  }
  ADM_assert(moov);
  while(!moov->isDone())
  {
    adm_atom son(moov);
    if(!ADM_mp4SearchAtomName(son.getFCC(), &id,&container))
    {
      adm_printf(ADM_PRINT_DEBUG,"Found atom %s unknown\n",fourCC::tostringBE(son.getFCC()));
    }
    else
    {
      switch( id)
      {
        case ADM_MP4_MVHD: parseMvhd(&son);break; 
        case ADM_MP4_TRACK: 
            if(!parseTrack(&son))
            {
                printf("Parse Track failed\n");
            } ;
            break;
        default : 
                adm_printf(ADM_PRINT_DEBUG,"atom %s not handled\n",fourCC::tostringBE(son.getFCC()));
                break;
      }
      
      
    }
    son.skipAtom();
  }
  delete moov;
  printf("Done finding main atoms\n");
  return 1;
}
/**
      \fn parseMvhd
      \brief Parse mvhd header
*/
void MP4Header::parseMvhd(void *ztom)
{
  adm_atom *tom=(adm_atom *)ztom;
  tom->skipBytes(12);
  uint32_t scale,duration=1000;
  
        scale=tom->read32();
        duration=tom->read32();
        _videoScale=scale;
        printf("Warning : scale is not in ms %lu !\n",_videoScale);
        if(_videoScale)
        {
                        duration=1000*duration; // In ms
                        duration/=_videoScale;
        }else
          _videoScale=1000;
        printf("Movie duration :%s\n",ms2timedisplay(duration));
        _movieDuration=duration;
}
/**
      \fn parseMvhd
      \brief Parse mvhd header
*/
uint8_t MP4Header::parseTrack(void *ztom)
{
  adm_atom *tom=(adm_atom *)ztom;
  ADMAtoms id;
  uint32_t container;
  uint32_t w,h;
  uint32_t trackType=TRACK_OTHER;
  
  printf("Parsing Track\n");
   while(!tom->isDone())
  {
     adm_atom son(tom);
     if(!ADM_mp4SearchAtomName(son.getFCC(), &id,&container))
     {
       adm_printf(ADM_PRINT_DEBUG,"Found atom %s unknown\n",fourCC::tostringBE(son.getFCC()));
       son.skipAtom();
       continue;
     }
     adm_printf(ADM_PRINT_DEBUG,"\tProcessing atom %s \n",fourCC::tostringBE(son.getFCC()));
     switch(id)
     {
       case ADM_MP4_TKHD:
              {
                      son.skipBytes(12);
                      adm_printf(ADM_PRINT_DEBUG,"Track Id: %lu\n",son.read32());
                      son.skipBytes(4);
                      adm_printf(ADM_PRINT_DEBUG,"Duration: %lu (ms)\n",(son.read32()*1000)/_videoScale);
                      son.skipBytes(8);
                      son.skipBytes(4); // layers
                      son.skipBytes(40); // layers
  
                      w=son.read32()>>16;
                      h=son.read32()>>16;
                      adm_printf(ADM_PRINT_DEBUG,"tkhd : %ld %ld\n",w,h);

              }
              break;
        case ADM_MP4_MDIA:
        {
            parseMdia(&son,&trackType,w,h);
            break;
        }
        case ADM_MP4_STBL:
        {
            parseMdia(&son,&trackType,w,h);
            break;
        }
       default:
          adm_printf(ADM_PRINT_DEBUG,"Unprocessed atom\n");
     }
     son.skipAtom();
  }
  return 1;
}
/**
      \fn parseMdia
      \brief Parse mdia header
*/
uint8_t MP4Header::parseMdia(void *ztom,uint32_t *trackType,uint32_t w, uint32_t h)
{
  adm_atom *tom=(adm_atom *)ztom;
  ADMAtoms id;
  uint32_t container;
  uint32_t trackScale=_videoScale;
  *trackType=TRACK_OTHER;
  uint8_t r=0;
  printf("<<Parsing Mdia>>\n");
  while(!tom->isDone())
  {
     adm_atom son(tom);
     if(!ADM_mp4SearchAtomName(son.getFCC(), &id,&container))
     {
       adm_printf(ADM_PRINT_DEBUG,"[MDIA]Found atom %s unknown\n",fourCC::tostringBE(son.getFCC()));
       son.skipAtom();
       continue;
     }
     switch(id)
     {
       case ADM_MP4_MDHD:  
       {
                uint32_t version=son.read(),duration;
                son.skipBytes(3); // flags + version
                son.skipBytes(4); // creation time
                son.skipBytes(4); // mod time
                if(version==1) son.skipBytes(8);
                trackScale=son.read32(); //
                adm_printf(ADM_PRINT_DEBUG,"MDHD,Trackscale in mdhd:%u\n",trackScale);
                if(!trackScale) trackScale=600; // default
                duration=son.read32();
                adm_printf(ADM_PRINT_DEBUG,"MDHD,duration in mdhd:%u (unscaled)\n",duration);
                duration=(uint32_t)((duration*1000.)/trackScale);
                adm_printf(ADM_PRINT_DEBUG,"MDHD,duration in mdhd:%u (scaled ms)\n",duration);
                printf("MDHD,Track duration :%s, trackScale :%u\n",ms2timedisplay(duration),trackScale);
                break;
       }
       case ADM_MP4_HDLR:  
       {
            uint32_t type;
            
                son.read32();
                son.read32();
                type=son.read32();
                printf("[HDLR]\n");
                switch(type)
                {	
                case MKFCCR('v','i','d','e')://'vide':
                        *trackType=TRACK_VIDEO;
                        printf("hdlr video found \n ");
                        break;
                case MKFCCR('s','o','u','n'): //'soun':
                        *trackType=TRACK_AUDIO;
                        printf("hdlr audio found \n ");
                        break;
                case MKFCCR('u','r','l',' ')://'url ':
                    {
                        int s;
                        son.read32();
                        son.read32();
                        son.read32();
                        s=son.read();
                        char str[s+1];
                        son.readPayload((uint8_t *)str,s);
                        str[s]=0;
                        printf("Url : <%s>\n",str);
                      }
                      break;
                 
                }
                break;
       } 
       case ADM_MP4_MINF:  
       {
            // We are only interested in stbl
            
            while(!son.isDone())
            {
              adm_atom grandson(&son);
              if(!ADM_mp4SearchAtomName(grandson.getFCC(), &id,&container))
              {
                adm_printf(ADM_PRINT_DEBUG,"[MINF]Found atom %s unknown\n",fourCC::tostringBE(son.getFCC()));
                grandson.skipAtom();
                continue;
              }
              if(id==ADM_MP4_STBL)
              {
                   if(! parseStbl(&grandson,*trackType, w, h,trackScale))
                   {
                      printf("STBL failed\n");
                      return 0; 
                   }
                   r=1;
              }
              grandson.skipAtom();
        }
       }
       break;
        default:
            adm_printf(ADM_PRINT_DEBUG,"** atom  NOT HANDLED [%s] \n",fourCC::tostringBE(son.getFCC()));
     }
     
     son.skipAtom();
  }
  return r;
}

/**
        \fn parseStbl
        \brief parse sample table. this is the most important function.
*/
uint8_t       MP4Header::parseStbl(void *ztom,uint32_t trackType,uint32_t w,uint32_t h,uint32_t trackScale)
{
  adm_atom *tom=(adm_atom *)ztom;
  ADMAtoms id;
  uint32_t container;
  MPsampleinfo  info;
  
  
  memset(&info,0,sizeof(info));

  
  printf("<<Parsing Stbl>>\n");
  while(!tom->isDone())
  {
     adm_atom son(tom);
     if(!ADM_mp4SearchAtomName(son.getFCC(), &id,&container))
     {
       adm_printf(ADM_PRINT_DEBUG,"[STBL]Found atom %s unknown\n",fourCC::tostringBE(son.getFCC()));
       son.skipAtom();
       continue;
     }
     switch(id)
     {
       case ADM_MP4_STSS:  // Sync sample atom (i.e. keyframes)
       {
          son.read32();
          info.nbSync=son.read32();
          printf("Stss:%u\n",info.nbSync);
          if(info.nbSync)
          {
                  info.Sync=new uint32_t[info.nbSync];
                  for(int i=0;i<info.nbSync;i++)
                  {
                          info.Sync[i]=son.read32();
                  }
          }
          break;
         
       }
       case ADM_MP4_STTS: 
            {
                printf("stts:%lu\n",son.read32()); // version & flags
                info.nbStts=son.read32();
                printf("Time stts atom found (%lu)\n",info.nbStts);
                printf("Using myscale %lu\n",trackScale);
                info.SttsN=new uint32_t[info.nbStts];
                info.SttsC=new uint32_t[info.nbStts];
                double dur;
                for(int i=0;i<info.nbStts;i++)
                {
                        
                        info.SttsN[i]=son.read32();
                        info.SttsC[i]=son.read32();
                        adm_printf(ADM_PRINT_VERY_VERBOSE,"stts: count:%u size:%u (unscaled)\n",info.SttsN[i],info.SttsC[i]);	
                        //dur*=1000.*1000.;; // us
                        //dur/=myScale;
                }                
            }
            break;
       case ADM_MP4_STSC:
            {
                son.read32();
                info.nbSc=son.read32();
                info.Sc=new uint32_t[info.nbSc];
                info.Sn=new uint32_t[info.nbSc];
                for(int j=0;j<info.nbSc;j++)
                {

                        info.Sc[j]=son.read32();
                        info.Sn[j]=son.read32();
                        son.read32();
                        adm_printf(ADM_PRINT_VERY_VERBOSE,"\t sc  %d : sc start:%lu sc count: %lu\n",j,info.Sc[j],info.Sn[j]);
                }

            }
            break;
       case ADM_MP4_STSZ:
          {
            uint32_t n;
              son.read32();
              n=son.read32();
              info.nbSz=son.read32();
              info.SzIndentical=0;
              printf("%lu frames /%lu nbsz..\n",n,info.nbSz);
              if(n)
                      {
                            adm_printf(ADM_PRINT_VERY_VERBOSE,"\t\t%lu frames of the same size %lu , n=%lu\n",
                                info.nbSz,info.SzIndentical,n);
                            info.SzIndentical=n;
                            info.Sz=NULL;
                      }
              else
              {
                      info.Sz=new uint32_t[info.nbSz];
                      for(int j=0;j<info.nbSz;j++)
                      {
                                      info.Sz[j]=son.read32();
                      }
              }
          }
          break;
       case ADM_MP4_STCO:
       {
          son.read32();
          info.nbCo=son.read32();
          printf("\t\tnbCo:%u\n",info.nbCo);
          info.Co=new uint32_t[info.nbCo];
          for(int j=0;j< info.nbCo;j++)
          {
                  info.Co[j]=son.read32();
                  adm_printf(ADM_PRINT_VERY_VERBOSE,"Chunk offset : %lu / %lu  : %lu\n",  j,info.nbCo,info.Co[j]);
          }

       }
       break;
       case ADM_MP4_STSD:
       {
                son.read32(); // flags & version
                int nbEntries=son.read32();
                int left;
                adm_printf(ADM_PRINT_DEBUG,"[STSD]Found %d entries\n",nbEntries);
                for(int i=0;i<nbEntries;i++)
                {
                   int entrySize=son.read32();
                   int entryName=son.read32();
                   left=entrySize-8;
                   if(i || (trackType==TRACK_VIDEO && _videoFound) || (trackType==TRACK_OTHER))
                   {
                    son.skipBytes(left); 
                    printf("[STSD] ignoring %s, size %u\n",fourCC::tostringBE(entryName),entrySize);
                    continue;
                   }
                   switch(trackType)
                   {
                     case TRACK_VIDEO:
                     {
                          uint32_t lw=0,lh=0;
                                printf("[STSD] VIDEO %s, size %u\n",fourCC::tostringBE(entryName),entrySize);
                                son.skipBytes(8);  // reserved etc..
                                left-=8;
                                son.read32(); // version/revision
                                left-=4;
                                printf("[STSD] vendor %s\n",fourCC::tostringBE(son.read32()));
                                left-=4;
                                
                                son.skipBytes(8); // spatial qual etc..
                                left-=8;
                                
                                printf("[STSD] width :%u\n",lw=son.read16());
                                printf("[STSD] height :%u\n",lh=son.read16());
                                left-=4;
                                
                                son.skipBytes(8); // Resolution
                                left-=8;
                                
                                printf("[STSD] datasize :%u\n",son.read32());
                                left-=4;
                     
                                printf("[STSD] FrameCount :%u\n",son.read16());
                                left-=4;
                                
                                // Codec name
                                uint32_t u32=son.read();
                                if(u32>31) u32=31;
                                printf("Codec string :%d <",u32);
                                for(int i=0;i<u32;i++) printf("%c",son.read());
                                printf(">\n");
                                son.skipBytes(32-1-u32);
                                left-=32;
                                // 
                                son.read32();
                                left-=4; //Depth & color Id
                                //
                                printf("LEFT:%d\n",left);
                                
                                if(left>8)
                                {
//                                  decodeVideoAtom(&son); 
                                }
                                //
#define commonPart(x)             _videostream.fccHandler=fourCC::get((uint8_t *)#x); 
                      

                                 _video_bih.biWidth=_mainaviheader.dwWidth=lw ; 
                                  _video_bih.biHeight=_mainaviheader.dwHeight=lh; 
                                  _video_bih.biCompression=_videostream.fccHandler;

                                //
                                switch(entryName)
                                {
                                  case MKFCCR('m','p','4','v'):  //mp4v
                                  {
                                        commonPart(DIVX);
                                         adm_atom esds(&son);
                                         printf("Reading esds, got %s\n",fourCC::tostringBE(esds.getFCC()));
                                         if(esds.getFCC()==MKFCCR('e','s','d','s'))
                                              decodeEsds(&esds,TRACK_VIDEO);
                                          left=0;
                                  }
                                        break;
                                  case MKFCCR('S','V','Q','3'):
                                  {//'SVQ3':
                                    // For SVQ3, the codec needs it to begin by SVQ3
                                    // We go back by 4 bytes to get the 4CC
                                            printf("SVQ3 atom found\n");
                                            VDEO.extraDataSize=left+4;
                                            VDEO.extraData=new uint8_t[ VDEO.extraDataSize ];
                                            if(!son.readPayload(VDEO.extraData+4,VDEO.extraDataSize-4 ))
                                            {
                                              GUI_Error_HIG(_("Problem reading SVQ3 headers"), NULL);
                                            }
                                            VDEO.extraData[0]='S';
                                            VDEO.extraData[1]='V';
                                            VDEO.extraData[2]='Q';
                                            VDEO.extraData[3]='3';
                                            printf("SVQ3 Header size : %lu",_videoExtraLen);
                                            commonPart(SVQ3);
                                            left=0;
                                  }
                                            break;
                                  case MKFCCR('d','v','c','p'): //'dv':
                                          commonPart(DVDS);
                                          break;
                                  case MKFCCR('h','2','6','3'): //'dv':
                                          commonPart(H263);
                                          break;
                                  case MKFCCR('M','J','P','G'): //'jpeg':
                                  case MKFCCR('j','p','e','g'): //'jpeg':
                                  case MKFCCR('A','V','D','J'): //'jpeg':
                                          commonPart(MJPG);
                                          break;
                                  case MKFCCR('a','v','c','1'): // avc1
                                          {
                                          commonPart(H264);
                                          // There is a avcC atom just after
                                          // configuration data for h264
                                          adm_atom avcc(&son);
                                          printf("Reading avcC, got %s\n",fourCC::tostringBE(avcc.getFCC()));
                                          int len,offset;
                                          VDEO.extraDataSize=avcc.getRemainingSize();
                                          VDEO.extraData=new uint8_t [VDEO.extraDataSize];
                                          avcc.readPayload(VDEO.extraData,VDEO.extraDataSize);
                                          printf("avcC size:%d\n",VDEO.extraDataSize);
                                    // Dump some info
                                        #define MKD8(x) VDEO.extraData[x]
                                        #define MKD16(x) ((MKD8(x)<<8)+MKD8(x+1))
                                        #define MKD32(x) ((MKD16(x)<<16)+MKD16(x+2))

                                            printf("avcC Revision             :%x\n", MKD8(0));
                                            printf("avcC AVCProfileIndication :%x\n", MKD8(1));
                                            printf("avcC profile_compatibility:%x\n", MKD8(2));
                                            printf("avcC AVCLevelIndication   :%x\n", MKD8(3));
        
                                            printf("avcC lengthSizeMinusOne   :%x\n", MKD8(4));
                                            printf("avcC NumSeq               :%x\n", MKD8(5));
                                            len=MKD16(6);
                                            printf("avcC sequenceParSetLen    :%x ",len );
                                            offset=8;
                                            mixDump(VDEO.extraData+offset,len);
        
                                            offset=8+len;
                                            printf("\navcC numOfPictureParSets  :%x\n", MKD8(offset++));
                                            len=MKD16(offset++);
                                            printf("avcC Pic len              :%x\n",len);
                                            mixDump(VDEO.extraData+offset,len);
                                            left=0;
                                            }
                                            break;
                                  default:
                                            if(left>10)
                                            {
                                                adm_atom avcc(&son);
                                                printf("Reading , got %s\n",fourCC::tostringBE(avcc.getFCC()));
                                                
                                            }
                                            break;
                                } // Entry name
                     }
                     break;
                     case TRACK_AUDIO:
                     {
                        uint32_t channels,bpp,encoding,fq,packSize;
                        
                                // Put some defaults
                                ADIO.encoding=1234;
                                ADIO.frequency=44100;
                                ADIO.byterate=128000>>3;
                                ADIO.channels=2;
                                ADIO.bitspersample=16;
                        
                                printf("[STSD] AUDIO %s, size %u\n",fourCC::tostringBE(entryName),entrySize);
                                son.skipBytes(8);  // reserved etc..
                                left-=8;
                                
                                int atomVersion=son.read16();  // version
                                left-=2;
                                printf("[STSD]Revision       :%d\n",atomVersion);
                                son.skipBytes(2);  // revision
                                left-=2;
                                
                                printf("[STSD]Vendor         : %s\n",fourCC::tostringBE(son.read32()));
                                left-=4;
                                
                                ADIO.channels=channels=son.read16(); // Channel
                                left-=2;
                                printf("[STSD]Channels       :%d\n",ADIO.channels);
                                ADIO.bitspersample=bpp=son.read16(); // version/revision
                                left-=2;
                                printf("[STSD]Bit per sample :%d\n",bpp);
                                
                                encoding=son.read16(); // version/revision
                                left-=2;
                                printf("[STSD]Encoding       :%d\n",encoding);

                                packSize=son.read16(); // Packet Size
                                left-=2;
                                printf("[STSD]Packet size    :%d\n",encoding);
                                
                                printf("[STSD]Left           :%d\n",left);
                                if(atomVersion<2)
                                {
                                        fq=ADIO.frequency=son.read16();
                                        printf("Fq:%u\n",fq);
                                        if(ADIO.frequency<6000) ADIO.frequency=48000;
                                        printf("[STSD]Fq       :%d\n",ADIO.frequency); // Bps
                                        son.skipBytes(2); // Fixed point
                                        left-=4;
                                }
                                switch(atomVersion)
                                {
                                  case 0:break;
                                  case 1:son.skipBytes(16);  // sample per frame etc..
                                          left-=16;
                                          break;
                                  case 2:
                                          ADIO.frequency=44100; // FIXME
                                          son.skipBytes(16);
                                          ADIO.channels=son.read32();
                                          printf("Channels            :%d\n",ADIO.channels); // Channels
                                          printf("Tak(7F000)          :%x\n",son.read32()); // Channels
                                          printf("Bits  per channel   :%d\n",son.read32());  // Vendor
                                          printf("Format specific     :%x\n",son.read32());  // Vendor
                                          printf("Byte per audio packe:%x\n",son.read32());  // Vendor
                                          printf("LPCM                :%x\n",son.read32());  // Vendor
                                          left-=(5*4+4+16);
                                          break;
                                }
                                printf("[STSD] chan:%u bpp:%u encoding:%u fq:%u (left %u)\n",channels,bpp,encoding,fq,left);
#define audioCodec(x) ADIO.encoding=WAV_##x;
                                switch(entryName)
                                {
                                    case MKFCCR('t','w','o','s'):
                                            audioCodec(LPCM);
                                            ADIO.byterate=ADIO.frequency*ADIO.bitspersample*ADIO.channels/8;
                                            break;
                                                
                                    case MKFCCR('u','l','a','w'):
                                            audioCodec(ULAW);
                                            ADIO.byterate=ADIO.frequency;
                                            break;
                                    case MKFCCR('s','o','w','t'):
                                            audioCodec(PCM);
                                            ADIO.byterate=ADIO.frequency*ADIO.bitspersample*ADIO.channels/8;
                                            break;
                                    case MKFCCR('.','m','p','3'): //.mp3
                                            audioCodec(MP3);
                                            ADIO.byterate=128000>>3;
                                            break;
                                    case MKFCCR('r','a','w',' '):
                                            audioCodec(8BITS_UNSIGNED);
                                            ADIO.byterate=ADIO.frequency*ADIO.channels;
                                            break;
                                    case MKFCCR('m','p','4','a'):
                                    {
                                            audioCodec(AAC);
                                            if(left>10)
                                            {
                                              adm_atom wave(&son);
                                              printf("Reading wave, got %s\n",fourCC::tostringBE(wave.getFCC()));
                                              if(MKFCCR('w','a','v','e')==wave.getFCC())
                                              {
                                                 // mp4a
                                                 //   wave
                                                 //     frma
                                                 //     mp4a
                                                 //     esds
                                                 while(!wave.isDone())
                                                 {
                                                     adm_atom item(&wave);
                                                     printf("parsing wave, got %s\n",fourCC::tostringBE(item.getFCC()));
                                                     switch(item.getFCC())
                                                     {
                                                       case MKFCCR('f','r','m','a'):
                                                          {
                                                          uint32_t codecid=item.read32();
                                                          printf("frma Codec Id :%s\n",fourCC::tostringBE(codecid));
                                                          }
                                                          break; 
                                                        case MKFCCR('m','p','4','a'):
                                                          break; 
                                                        case MKFCCR('e','s','d','s'):
                                                          {
                                                               decodeEsds(&item,TRACK_AUDIO);
                                                          goto foundit; // FIXME!!!
                                                          }
                                                          break;
                                                       default:
                                                         break;
                                                     }

                                                     item.skipAtom();
                                                   
                                                 }  // Wave iddone
                                              }  // if ==wave
                                              
                                            } // if left > 10
foundit: // HACK FIXME     
                                            left=0;
                                    }       
                                            break; // mp4a
                                  
                                }
                     }
                          break;
                     default:
                          ADM_assert(0);
                   }
                   son.skipBytes(left); 
                }
       }
              break;
       default:
          printf("[STBL]Skipping atom %s\n",fourCC::tostringBE(son.getFCC()));
     }
     son.skipAtom();
  }
  uint8_t r=0;
  uint32_t nbo=0;
  switch(trackType)
  {
    case TRACK_VIDEO:
          if(_tracks[0].index)
          {
              printf("Already got a video track\n");
              return 1;
          }
          r=indexify(&(_tracks[0]),trackScale,&info,0,&nbo);
          _videostream.dwLength= _mainaviheader.dwTotalFrames=_tracks[0].nbIndex;
          // if we have a sync atom ???
          if(info.nbSync)
          {
            // Mark keyframes
            for(int i=0;i<info.nbSync;i++)
            {
              int sync=info.Sync[i];
              if(sync) sync--;
              _tracks[0].index[sync].intra=AVI_KEY_FRAME;
            }
          }else
          { // All frames are kf
            for(int i=0;i<_tracks[0].nbIndex;i++)
            {
              _tracks[0].index[i].intra=AVI_KEY_FRAME;
            }
            
          }
           VDEO.index[0].intra=AVI_KEY_FRAME;
          break;
    case TRACK_AUDIO:
          printf("Cur audio track :%u\n",nbAudioTrack);
          if(info.SzIndentical ==1 && (ADIO.encoding==WAV_LPCM || ADIO.encoding==WAV_PCM ))
            {
              printf("Overriding size %lu -> %lu\n", info.SzIndentical,info.SzIndentical*2*ADIO.channels);
              info.SzIndentical=info.SzIndentical*2*ADIO.channels;
            }
            r=indexify(&(_tracks[1+nbAudioTrack]),trackScale,&info,1,&nbo);
            printf("Indexed audio, nb blocks:%u\n",nbo);
            if(r)
            {
                nbo=_tracks[1+nbAudioTrack].nbIndex;
                if(nbo)
                    _tracks[1+nbAudioTrack].nbIndex=nbo;
                else
                    _tracks[1+nbAudioTrack].nbIndex=info.nbSz;
                printf("Indexed audio, nb blocks:%u (final)\n",_tracks[1+nbAudioTrack].nbIndex);
                nbAudioTrack++;
            }
            
            break;
    case TRACK_OTHER:
        r=1;
        break;
  }
  return r;
}
/**
        \fn indexify
        \brief build the index from the stxx atoms
*/
uint8_t	MP4Header::indexify(
                          MP4Track *track,   
                          uint32_t trackScale,
                         MPsampleinfo *info,
                         uint32_t isAudio,
                         uint32_t *outNbChunk)

{

uint32_t i,j,cur;

        printf("Build Track index\n");
	*outNbChunk=0;
	aprintf("+_+_+_+_+_+\n");
	aprintf("co : %lu sz: %lu sc: %lu co[0]%lu \n",info->nbCo,info->nbSz,info->nbSc,info->Co[0]);
	aprintf("+_+_+_+_+_+\n");

	ADM_assert(info->Sc);
	ADM_assert(info->Sn);
	ADM_assert(info->Co);
	if(!info->SzIndentical)
        {
          ADM_assert(info->Sz);
        }
	// first set size
	if(info->SzIndentical && isAudio)// in that case they are all the same size, i.e.audio
	{
          
          
          uint32_t totalBytes=info->SzIndentical*info->nbSz;
          printf("All the same size : %u (total size %u bytes)\n",info->SzIndentical,totalBytes);
              //
              // Each chunk contains N samples=N bytes
              int samplePerChunk[info->nbCo];
              memset(samplePerChunk,0,info->nbCo*sizeof(int));
              for( i=0;i<info->nbSc;i++)
              {
                  for(int j=info->Sc[i]-1;j<info->nbCo;j++)
                  {
                        adm_printf(ADM_PRINT_VERY_VERBOSE,"For chunk %lu , %lu samples\n",j,info->Sn[i]);
                        samplePerChunk[j]=info->Sn[i];
                  }
              }
              int total=0;
              for( i=0;i<info->nbCo;i++)
              {
                  adm_printf(ADM_PRINT_VERY_VERBOSE,"%u -> %u samples %u bytes\n",i,samplePerChunk[i],samplePerChunk[i]*info->SzIndentical);
                  total+=samplePerChunk[i];
              }
              printf("Total size in byte : %u\n",total*info->SzIndentical);
              track->index=new MP4Index[info->nbCo];
              memset(track->index,0,info->nbCo*sizeof(MP4Index));
              track->nbIndex=info->nbCo;;
              int max=0;
              totalBytes=0;
              for(i=0;i<info->nbCo;i++)
              {
                  uint32_t sz;

                  track->index[i].offset=info->Co[i];
                  sz=samplePerChunk[i]*info->SzIndentical;
                  track->index[i].size=sz;
                  track->index[i].time=0; // No seek
                  if(sz>MAX_CHUNK_SIZE)
                  {
                      max+=sz/MAX_CHUNK_SIZE;
                  }
                  
                  totalBytes+=track->index[i].size;
              }
              // Now time to update the time...
              // Normally they have all the same duration
              if(info->nbStts!=1) printf("WARNING: Same size, different duration\n");

              float sampleDuration,totalDuration=0;
              
                sampleDuration=info->SttsC[0];
                sampleDuration*=1000.*1000.;
                sampleDuration/=trackScale;    // Duration of one sample
                for(i=0;i<info->nbCo;i++)
                {
                        track->index[i].time=(uint64_t)floor(totalDuration);
                        totalDuration+=sampleDuration*samplePerChunk[i];
                        adm_printf(ADM_PRINT_VERY_VERBOSE,"Audio chunk : %lu time :%lu\n",i,track->index[i].time);
                }
                if(max && isAudio) // We have some big chunks we need to split them
                {
                      // rebuild a new index
                      printf("We have %u chunks that are too big, adjusting..\n",max);
                      uint32_t newNbCo=track->nbIndex+max*2; // *2 is enough, should be.
                      uint32_t w=0;
                      uint32_t one_go;

                        one_go=MAX_CHUNK_SIZE/info->SzIndentical;
                        one_go=one_go*info->SzIndentical;

                     MP4Index *newindex=new MP4Index[newNbCo];

                    int64_t time_increment=(int64_t)((one_go/info->SzIndentical)*sampleDuration);  // Nb sample*duration of one sample
                    for(i=0;i<track->nbIndex;i++)
                    {
                      uint32_t sz;
                          sz=track->index[i].size;
                          if(sz<MAX_CHUNK_SIZE)
                          {
                              memcpy(&(newindex[w]),&(track->index[i]),sizeof(MP4Index));
                              w++;
                              continue;
                          }
                          // We have to split it...
                          int part=0;
                          while(sz>one_go)
                          {
                                newindex[w].offset=track->index[i].offset+part*one_go;
                                newindex[w].size=one_go;
                                newindex[w].time=track->index[i].time+part*time_increment; 
                                ADM_assert(w<newNbCo);
                                w++;
                                part++;
                                sz-=one_go;
                          }
                          // The last one...
                                newindex[w].offset=track->index[i].offset+part*one_go;
                                newindex[w].size=sz;
                                newindex[w].time=track->index[i].time+part*time_increment+((time_increment*sz)/one_go); 
                                w++;
                    }
                    delete [] track->index;
                    track->index=newindex;
                    track->nbIndex=w;
                }
          printf("Index done\n");
          return 1;
      }
          // Else we build an index per sample
          //
	
		
	// We have different packet size
	// Probably video
        track->index=new MP4Index[info->nbSz];
        memset(track->index,0,info->nbSz*sizeof(MP4Index));

        if(info->SzIndentical) // Video, all same size (DV ?)
        {
            adm_printf(ADM_PRINT_VERY_VERBOSE,"\t size for all %lu frames : %lu\n",info->nbSz,info->SzIndentical);
            for(i=0;i<info->nbSz;i++)
            {
                    track->index[i].size=info->SzIndentical;
                    
            }
          }
          else // Different size
          {
            for(i=0;i<info->nbSz;i++)
            {
                    track->index[i].size=info->Sz[i];
                    adm_printf(ADM_PRINT_VERY_VERBOSE,"\t size : %d : %lu\n",i,info->Sz[i]);
            }
          }
	// if no sample to chunk we map directly
	// first build the # of sample per chunk table
        uint32_t totalchunk=0,max=0;

        // Search the maximum
        for(i=0;i<info->nbSc-1;i++)
        {
                totalchunk+=(info->Sc[i+1]-info->Sc[i])*info->Sn[i];
        }
        totalchunk+=(info->nbCo-info->Sc[info->nbSc-1]+1)*info->Sn[info->nbSc-1];

        adm_printf(ADM_PRINT_VERY_VERBOSE,"#of chunk %d max per chunk %d Max # of sample %d\n",info->nbCo,max,totalchunk);

        uint32_t chunkCount[totalchunk+1];
	for(i=0;i<info->nbSc;i++)
	{
		for(j=info->Sc[i]-1;j<info->nbCo;j++)
		{
			chunkCount[j]=info->Sn[i];
                        ADM_assert(j<=totalchunk);
		}
		adm_printf(ADM_PRINT_VERY_VERBOSE,"(%d) sc: %lu sn:%lu\n",i,info->Sc[i],info->Sn[i]);
	}
/*			for(j=0;j<nbSc;j++)
			{
				aprintf("\n count number : %d - %lu\n",j,Sn[j]);
			}*/
	// now we have for each chunk the number of sample in it
	cur=0;
	for(j=0;j<info->nbCo;j++)
	{
		int tail=0;
		adm_printf(ADM_PRINT_VERY_VERBOSE,"--starting at %lu , %lu to go\n",info->Co[j],chunkCount[j]);
		for(uint32_t k=0;k<chunkCount[j];k++)
		{
                        track->index[cur].offset=info->Co[j]+tail;
                        tail+=track->index[cur].size;
                        adm_printf(ADM_PRINT_VERY_VERBOSE," sample : %d offset : %lu\n",cur,track->index[cur].offset);
			adm_printf(ADM_PRINT_VERY_VERBOSE,"Tail : %lu\n",tail);
			cur++;
		}


	}
        
        
        track->nbIndex=cur;;
	
	
	// Now deal with duration
	// the unit is us FIXME, probably said in header
	// we put each sample duration in the time entry
	// then sum them up to get the absolute time position

        uint32_t nbChunk=track->nbIndex;
	if(info->nbStts)		//uint32_t nbStts,	uint32_t *SttsN,uint32_t SttsC,
	{
		uint32_t start=0;
		if(info->nbStts>1)
		{
			for(uint32_t i=0;i<info->nbStts;i++)
			{
				for(uint32_t j=0;j<info->SttsN[i];j++)
				{
                                        track->index[start].time=(uint64_t)info->SttsC[i];
					start++;
					ADM_assert(start<=nbChunk);
				}	
			}
		}
		else
		{
			// All same duration
			for(uint32_t i=0;i<nbChunk;i++)
                                track->index[i].time=(uint64_t)info->SttsC[0]; // this is not an error!
		
		}
		// now collapse
		uint64_t total=0;
		float    ftot;
		uint32_t thisone;
		
		for(uint32_t i=0;i<nbChunk;i++)
		{
                        thisone=track->index[i].time;
			ftot=total;
			ftot*=1000.*1000.;
			ftot/=trackScale;
                        track->index[i].time=(uint64_t)floor(ftot);
			total+=thisone;
                        adm_printf(ADM_PRINT_VERY_VERBOSE,"Audio chunk : %lu time :%lu\n",i,track->index[i].time);
		}
		// Time is now built, it is in us
	
	
	}
	else // there is not ssts
	{
          GUI_Error_HIG(_("No stts table"), NULL);
		ADM_assert(0);	
	}
        printf("Index done\n");
	return 1;
}
/**
      \fn decodeEsds
      \brief Decode esds atom
*/
uint8_t MP4Header::decodeEsds(void *ztom,uint32_t trackType)
{
adm_atom *tom=(adm_atom *)ztom; 
int tag,l;
            // in case of mpeg4 we only take
            // the mpeg4 vol header
            printf("Esds atom found\n");

            tom->skipBytes(4);
            tag=0xff;
            while(tag!=Tag_DecSpecificInfo && !tom->isDone())
            {
                    tag=tom->read();
                    l=readPackedLen(tom);
                    printf("\t Tag : %u Len : %u\n",tag,l);
                    switch(tag)
                    {
                            case Tag_ES_Desc:
                                    printf("\t ES_Desc\n");
                                    tom->skipBytes(3);
                                    break;
                            case Tag_DecConfigDesc:
                            {
                                    uint8_t objectTypeIndication=tom->read();
                                    printf("\tDecConfigDesc : Tag %u\n",objectTypeIndication);
                                    if(trackType==TRACK_AUDIO && ADIO.encoding==WAV_AAC)
                                    {
                                      switch(objectTypeIndication)
                                      {
                                          case 0x69:ADIO.encoding=WAV_MP3;break;
                                          case 0x6b:ADIO.encoding=WAV_MP3;break;
                                          case 0x6d:ADIO.encoding=WAV_MP3;break;
                                          case 226:ADIO.encoding=WAV_AC3;break;
                                          break;
                                  
                                      }
                                    }
                                    tom->skipBytes(1+3+4+4);	
                                    break;
                            }
                            case Tag_DecSpecificInfo:
                                    printf("\t DecSpecicInfo\n");
                                    switch(trackType)
                                    {
                                        case TRACK_VIDEO: // Video
                                            if(!VDEO.extraDataSize)
                                            {
                                                    VDEO.extraDataSize=l;
                                                    VDEO.extraData=new uint8_t[l];
                                                    fread(VDEO.extraData,VDEO.extraDataSize,1,_fd);
                                            }
                                            break;
                                        case TRACK_AUDIO:
                                            printf("Esds for audio\n");
                                            _tracks[1+nbAudioTrack].extraDataSize=l;
                                            _tracks[1+nbAudioTrack].extraData=new uint8_t[l];
                                            fread(_tracks[1+nbAudioTrack].extraData,
                                                _tracks[1+nbAudioTrack].extraDataSize,1,_fd);
                                            break;
                                        default: printf("Unknown track type for esds %d\n",trackType);
                                    }
                            }
            }
            
    tom->skipAtom();
    return 1;
}
//***********************************
MPsampleinfo::MPsampleinfo(void)
{
  memset(this,0,sizeof( MPsampleinfo));
}
MPsampleinfo::~MPsampleinfo()
{
#define MPCLEAR(x) {if(x) delete [] x;x=NULL;}
      MPCLEAR (Co);
      MPCLEAR (Sz);
      MPCLEAR (Sc);
      MPCLEAR (Sn);
      MPCLEAR (SttsN);
      MPCLEAR (SttsC);
      MPCLEAR (Sync);
}

// EOF
