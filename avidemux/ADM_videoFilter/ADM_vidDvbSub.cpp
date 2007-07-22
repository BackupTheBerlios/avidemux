/***************************************************************************
                         DVB-T subtitle filter
    
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
#include "ADM_lavcodec.h"
#include "ADM_assert.h"
#include "default.h"

#include "fourcc.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"
#include "ADM_filter/video_filters.h"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"

#include "DIA_factory.h"
#include "ADM_inputs/ADM_mpegdemuxer/dmx_mpegstartcode.h"
#include "ADM_inputs/ADM_mpegdemuxer/dmx_demuxerTS.h"

#include "ADM_colorspace/colorspace.h"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_ffmp43.h"

#define READ_BUFFER_SIZE (64*1024)

class ADMVideoSubDVB : public AVDMGenericVideoStream 
{
protected:
        virtual char* printConf(void);
       
        uint8_t init(void);
        
        decoderFFSubs *decoder;
        ADMCompressedImage *binary;
        dmx_demuxerTS *demuxer;
        AVSubtitle  sub;
        uint8_t     readBuffer[READ_BUFFER_SIZE];
        uint32_t    _inited;
        
public:
        ADMVideoSubDVB(AVDMGenericVideoStream *in, CONFcouple *conf);
        virtual ~ADMVideoSubDVB();
        virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, ADMImage *data,uint32_t *flags);
        uint8_t configure(AVDMGenericVideoStream *instream);
        uint8_t	getCoupledConf(CONFcouple **conf);
};

static FILTER_PARAM assParam={7,
        { /* float */ "font_scale",
          /*float*/ "line_spacing",
          /* int */ "top_margin",
          /* int */ "bottom_margin",
          /* bool */ "extract_embedded_fonts",
          /* ADM_filename */ "fonts_dir",
          /* ADM_filename */ "subfile" }};

SCRIPT_CREATE(dvbsub_script,ADMVideoSubDVB,assParam);
BUILD_CREATE(dvbsub_create,ADMVideoSubDVB);

char *ADMVideoSubDVB::printConf() 
{
      static char buf[50];
      sprintf((char *)buf," DVB Subtitle ");
      
      return buf;
}


uint8_t ADMVideoSubDVB::configure(AVDMGenericVideoStream * instream)
{
  
   return 1;
}

//_______________________________________________________________

ADMVideoSubDVB::ADMVideoSubDVB(AVDMGenericVideoStream *in, CONFcouple *conf) 
{
        _in=in;		
        memcpy(&_info,_in->getInfo(),sizeof(_info));
        
        
      
        _uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
        ADM_assert(_uncompressed);
        _info.encoding=1;
        
        decoder=new decoderFFSubs(1);
        binary=new ADMCompressedImage;
        binary->data=readBuffer+2;
        
        MPEG_TRACK track;
        track.pid=0x96;

        
        demuxer=new dmx_demuxerTS(1,&track,0,DMX_MPG_TS);
        _inited=init();

      
}
// **********************************
uint8_t ADMVideoSubDVB::init(void)
{
const char  *tsFileName="/capture/grey/Grey_anatomy_2007_05_22_Avec_Le_Temp.mpg";
    // First create our demuxer
     
     if(!demuxer->open(tsFileName))
     {
        printf("Cannot open TS\n");
        return 0; 
     }
      memset(&sub,0,sizeof(sub));
     
     return 1;
} 

//*******************************************
ADMVideoSubDVB::~ADMVideoSubDVB() 
{
      if(_uncompressed) DELETE(_uncompressed);
      if(decoder) delete decoder;
      if(binary) delete binary;
      if(demuxer) delete demuxer;
      decoder=NULL;
      binary=NULL;
      demuxer=NULL;
   
}
//*******************************************
#define _r(c)  ((c)&0xff)
#define _b(c)  (((c)>>16)&0xFF)
#define _g(c)  (((c)>>8)&0xFF)
#define _a(c)  ((c)>>24)
#define rgba2y(c)  ( (( 263*_r(c)  + 516*_g(c) + 100*_b(c)) >> 10) + 16  )
#define rgba2u(c)  ( (( 450*_r(c) - 376*_g(c) -  73*_b(c)) >> 10) + 128 )
#define rgba2v(c)  ( ((-152*_r(c) - 298*_g(c) + 450*_b(c)) >> 10) + 128 )

uint8_t ADMVideoSubDVB::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, ADMImage *data, uint32_t *flags) 
{
uint8_t *org=NULL;

      // Read the original PIC
        
        if(!_in->getFrameNumberNoAlloc(frame, len, data, flags))
                return 0;

        if(!_inited) 
        {
          return 0; 
        }
        // Read the compressed DVB....
        
        uint32_t packetLen,dts,pts;
          if(!demuxer->readPes(readBuffer,&packetLen, &dts,&pts)) return 1;
          binary->dataLength=packetLen-3; // -2 for stream iD, -1 for ????
          if(packetLen<=5) return 1;
          // And decompress...
          decoder->uncompress(binary,&sub);
            
          // Process All rectangles
          
            printf("Found %d rects to process\n",sub.num_rects);
            for(int i=0;i<sub.num_rects;i++)
            {
              AVSubtitleRect *r=&(sub.rects[i]);
              // First convert RGB to Y+ALPHA
              for(int col=0;col<r->nb_colors;col++)
              {
                    // Color is RGB, convert to YUV
                    uint32_t y,a;
                    uint32_t rgb=r->rgba_palette[col];
                
                          y=rgba2y(rgb);
                          a=_a(rgb);
                          r->rgba_palette[col]=y+(a<<8);
                          printf("Color %d, alpha %u luma %u rgb:%x\n",col,a,y,rgb);
              }
              // Palette is ready, display !
              if(r->x>_info.width || r->y>_info.height)
              {
                  printf("[DVBSUB]Box is outside image\n");
                  goto _skip;
              }
                  printf("x :%d\n",r->x);
                  printf("y :%d\n",r->y);
                  printf("w :%d\n",r->w);
                  printf("h :%d\n",r->h);

                  {
                      uint32_t clipW,clipH;
                      
                      clipW=FFMIN(_info.width,r->x+r->w)-r->x;
                      clipH=FFMIN(_info.height,r->y+r->h)-r->y;
                      
                      ADMImage image(r->w,r->h);
                      ADMImage alphaImage(r->w,r->h);
                      
                      uint8_t *ptr=image.data;
                      uint8_t *ptrAlpha=alphaImage.data;
                      uint8_t *in=r->bitmap;
                      for(int yy=0;yy<r->h;yy++)
                      {
                          for(int xx=0;xx<r->w;xx++)
                          {
                            uint32_t alpha;
                            uint32_t val=*in++;
                            
                                  val=r->rgba_palette[val];
                                  alpha=val>>8;
                                  val&=0xff;
                                  
                                  *ptr++=val;
                                  *ptrAlpha++=alpha;
                          }
                      }
              
                      // Merge Luma
                      
                      for(int yy=0;yy<clipH;yy++)
                      {
                          org=data->data+(yy+r->y)*_info.width+r->x;
                          
                          ptrAlpha=alphaImage.data+yy*r->w;
                          ptr=image.data+yy*r->w;
                        
                          for(int xx=0;xx<clipW;xx++)
                          {
                            uint32_t val,before,alpha;
                            
                                  before=*org;
                                  val=*ptr++;
                                  alpha=*ptrAlpha++;
                                  
                                  val=val*alpha+(255-alpha)*before;
                                  val>>=8;
                                  *org++=val;
                          }
                      }
              }
               // Delete palette & data
_skip:
               av_free(r->rgba_palette);
               av_free(r->bitmap);
            } // Next rec..
            memset(&sub,0,sizeof(sub));
        
        return 1;
}

uint8_t	ADMVideoSubDVB::getCoupledConf(CONFcouple **conf) 
{

        *conf=NULL; //new CONFcouple(0);
#if 0
#define _COUPLE_SET(x) (*conf)->setCouple(#x, _params->x);
        _COUPLE_SET(font_scale)
        _COUPLE_SET(line_spacing)
        _COUPLE_SET(top_margin)
        _COUPLE_SET(bottom_margin)
        _COUPLE_SET(subfile)
        _COUPLE_SET(fonts_dir)
        _COUPLE_SET(extract_embedded_fonts)
#endif
     
        return 1;
}
/************************************************/
