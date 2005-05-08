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
//
// C++ Implementation:Spudecoder (subs for DVd like stream)
//
// Description:
//
//
// Author: Mean, fixounet@free.fr
//
// Copyright: See COPYING file that comes with this distribution
//
// see http://sam.zoy.org/writings/dvd/subtitles/

/*
        This part of the vobsub stuff 
                - decodes the command spu block
                - rle decode the bitmap itself
                
        
       palettized is the palette based decoded sub
       
       Elsewhere
       
       bitmap & alphamask are the bitmap & alpha channel stuff
       subW & subH are width and height of them
       
       The RLE decoder is derivated from mplayer one
*/
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
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFlipV.h"
#include "ADM_filter/video_filters.h"

#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"

#include "ADM_toolkit/filesel.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_vobsubinfo.h"

#include "ADM_vidVobSub.h"

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
extern "C" {
#include "../../adm_lavcodec/avcodec.h"
}
#endif

#include "ADM_video/swscale.h"
#include "ADM_toolkit/ADM_cpuCap.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"

//*************************************************************
uint8_t ADMVideoVobSub::forward(uint32_t v)
{
        aprintf("Current : %lu forwarding : %lu\n",_curOffset,v);
         ADM_assert(_curOffset+v<_subSize);
         _curOffset+=v;
         return 1;

}
//*************************************************************
uint8_t ADMVideoVobSub::readbyte( void )
{
        ADM_assert(_curOffset<_subSize);
        return _data[_curOffset++];

}
//*************************************************************
uint16_t ADMVideoVobSub::readword( void )
{
uint16_t w;
        ADM_assert(_curOffset<_subSize-1);
        w= _data[_curOffset]<<8;
        w+= _data[_curOffset+1];
        _curOffset+=2;
        return w;
}
//***********************************************************
//      Convert RGB Palette to YUV Palette
//      ALL 16 of them
//
uint8_t ADMVideoVobSub::paletteYUV( void )
{
uint8_t r,g,b,a;
uint8_t y;
int8_t u,v;
uint32_t value;
        for(int i=0;i<16;i++)
        {
               value=_vobSubInfo->Palette[i];
               r=(value>>16)&0xff;
               g=(value>>8)&0xff; 
               b=(value)&0xff;
               
               COL_RgbToYuv(b,  g,  r, &y, &u,&v);
               
                _YUVPalette[i]=y;
                //_YUVPalette[i][1]=u;
                //_YUVPalette[i][2]=v;
        
        }
        return 1;

}


//***********************************************************
// RLE code inspired from mplayer
uint8_t ADMVideoVobSub::decodeRLE(uint32_t off,uint32_t start,uint32_t end)
{

        if(!_original) return 0;

        uint32_t oldoffset=_curOffset;
        uint32_t stride=_subW;
        uint32_t x,y;
        uint8_t *ptr=_original->_bitmap;
        uint8_t *alpha=_original->_alphaMask;
        uint32_t a,b;
        int     nibbleparity=0;
        int     nibble=0;
        
        int run,color;
        
#define SKIPNIBBLE        {nibbleparity=0;}
#define NEXTNIBBLE(x) if(nibbleparity){ x=nibble&0xf;nibbleparity=0;}else {nibble=readbyte();nibbleparity=1;x=nibble>>4;}
       
        _curOffset=off;
        aprintf("Vobsub: Rle at offset :%d datasize:%d (stride:%d)\n",off,_dataSize,stride);
        if(!ptr)
        {
                printf("Vobsub:No infos yet RLE data...\n");
                 return 1;
        }
        x=0;
        y=0;
        while(
               (_curOffset<_dataSize)
            && (y<(_subH>>1)) 
            && ((!end) || (_curOffset<end))
        )
        {
               NEXTNIBBLE(a);
               if(a<4)
               {
                 a<<=4;
                 NEXTNIBBLE(b);
                 a|=b;
                 if(a<0x10)
                 {
                        a<<=4;
                        NEXTNIBBLE(b);
                        a|=b;
                        if(a<0x40)
                        {
                                a<<=4;
                                NEXTNIBBLE(b);
                                a|=b;
                                if(a<0x100)
                                {
                                        a|=(stride-x)<<2;
                                }
                        }
                 }
              }
              run=a>>2;
              color=3-(a&0x3);
             // aprintf("Vobsub: Run:%d color:%d\n",run,color);
              if((run>stride-x) || !run)
                run=stride-x;
              
              //memset(ptr,color,run);
              memset(ptr,_colors[color],run);
              memset(alpha,_alpha[color],run);
              if(run!=stride) _original->setDirty(y*2+start);
              x+=run;
              ptr+=run;
              alpha+=run;
              //  aprintf("x:%d y:%d\n",x,y);
              if(x>=stride)
              {
                        
                     y++;
                     x=0;
                     ptr=_original->_bitmap+(y*2+start)*stride;
                     alpha=_original->_alphaMask+(y*2+start)*stride;
                     SKIPNIBBLE;
              }
        }
        aprintf("vobsub End :%d y:%d\n",_curOffset,y); 
        _curOffset=oldoffset;
} 
//***********************************************        
// Decode Sub automaton command
//***********************************************
uint8_t  ADMVideoVobSub::handleSub( uint32_t idx )
{       
uint16_t date,next,dum;
uint8_t  command;
        _subSize=0;
uint32_t pts;
        // Read data
aprintf("**Cur:%llx next:%llx\n",_parser->getAbsPos(),_vobSubInfo->lines[idx+1].fileOffset);        
while(_parser->getAbsPos()+8<_vobSubInfo->lines[idx+1].fileOffset)
{        
        aprintf("**Cur:%llx next:%llx\n",_parser->getAbsPos(),_vobSubInfo->lines[idx+1].fileOffset);
        _subSize=_parser->read16i();
        if(!_subSize)
        {
            printf("Vobsub: error reading\n");
            return 0;
        }
        
        aprintf("Vobsub: data len =%d\n",_subSize);
        
        if(_subSize>VS_MAXPACKET-1)
         {
            printf("Vobsub: error reading (packet too big)\n");
            return 0;
        }
        if(!_parser->read(_data+2,_subSize-2)) 
        {
                printf("VS: read failed\n");
                return 0;
        }
       
        // We got the full packet
        // now scan it
        _curOffset=2;
        _dataSize=readword();
        aprintf("data block=%lu\n",_dataSize);
        if(_dataSize<=4)
        {
            printf("Vobsub: data block too small\n");
            return 0;       
        }
        
        forward(_dataSize-4);    // go to the command block
        
        
        while(2)
        {
                if(_curOffset>_subSize-5) break;
                date=readword();
                next=readword();
                if(next==_curOffset-4) break;            // end of command
                
                while(_curOffset<next)
                {
                        command=readbyte();
                        aprintf("vobsub:Command : %d date:%d next:%d cur:%lu\n",command,date,next,_curOffset);
                        switch(command)
                        {
                                case 00: _displaying=1;
                                        break;
                                case 01: // start date
                                        break;
                                case 02: // stop date
                                        pts=_parser->getPTS();
                                        if(!_vobSubInfo->lines[idx].stopTime)
                                        {
                                                double comp;
                                           
                                                comp=pts-_initialPts;
                                                comp=comp/90;     // 90khz
                                                comp+=date*10;    // 1/100th of a second
                                                 _vobSubInfo->lines[idx].stopTime=(uint32_t)comp;
                                                aprintf("****Sub: idx : %lu starts at :%lu end at :%lu\n",
                                                        idx,
                                                        _vobSubInfo->lines[idx].startTime, 
                                                        _vobSubInfo->lines[idx].stopTime);
                                        
                                        }
                                        break;
                                case 03: // Pallette 4 nibble= 16 bits
                                         dum=readword();
                                        _colors[0]=dum>>12;
                                        _colors[1]=0xf & (dum>>8);
                                        _colors[2]=0xf & (dum>>4);
                                        _colors[3]=0xf & (dum);
                                        break;          
                                case 0xff:
                                        break;
                                case 04: // alpha channel
                                         //4 nibble= 16 bits
                                        dum=readword();
                                        _alpha[0]=dum>>12;
                                        _alpha[1]=0xf & (dum>>8);
                                        _alpha[2]=0xf & (dum>>4);
                                        _alpha[3]=0xf & (dum);
                                        break;
                                case 05:
                                        // Coordinates 12 bits per entry X1/X2/Y1/Y2
                                        // 48 bits total / 6 bytes
                                        {
                                                uint16_t a,b,c;
                                                uint32_t nx1,nx2,ny1,ny2;
                                                
                                                a=readword();
                                                b=readword();
                                                c=readword();
                                                nx1=a>>4;
                                                nx2=((a&0xf)<<8)+(b>>8);
                                                ny1=((b&0xf)<<4)+(c>>12);
                                                ny2=c&0xfff;
                                                
                                                aprintf("vobsuv: x1:%d x2:%d y1:%d y2:%d\n",nx1,nx2,ny1,ny2);
                                                
                                                if(nx1==_x1 && nx2==_x2 && ny1==_y1 && ny2==_y2 && _original)
                                                {       // Reuse old bitmap
                                                        _original->clear();
                                                }
                                                else
                                                {
                                                  if(_original)
                                                        delete _original;
                                                  _original=NULL;
                                                  _x1=nx1;
                                                  _x2=nx2;
                                                  _y1=ny1;
                                                  _y2=ny2;
                                                  _subW=_x2-_x1+1;
                                                  _subH=_y2-_y1+1;                                                
                                                  _original=new vobSubBitmap(_subW,_subH);
                                                }
                                                                        
                                        }
                                        break;
                                case 06: // RLE offset 
                                        // 2*16 bits : odd offset, even offset
                                        {
                                        uint32_t odd,even;
                                        odd=readword();                                        
                                        even=readword();
                                        if(_original) 
                                                {
                                                _original->clear();
                                                decodeRLE(odd,0,even);
                                                decodeRLE(even,1,0);
                                                }
                                        }
                                        break;   
                                default:                                                     
                                        printf("Unknown command:%d\n",command);
                                        return 0;
                                  
                        } //End switch command     
                }// end while
        }
  }     
}
vobSubBitmap::vobSubBitmap(uint32_t w, uint32_t h)
{
  uint32_t page;
  
  _width=w;
  _height=h;
  
  page=w*h;
   

  _bitmap=new uint8_t [page];
  _alphaMask=new uint8_t [page];
  _dirty=new uint8_t[h];                                                
  clear();
}
uint8_t vobSubBitmap::isDirty(uint32_t line)
{
        ADM_assert(line<_height);
        if(_dirty[line]) return 1;
        return 0;

}
uint8_t vobSubBitmap::setDirty(uint32_t line)
{
        ADM_assert(line<_height);
        _dirty[line]=1;
        return 1;
}
vobSubBitmap::~vobSubBitmap()
{
#define CLN(x) if(x) delete [] x;

  CLN(_bitmap);
  CLN(_alphaMask);  
  CLN(_dirty);
}
void vobSubBitmap::clear(void)
{
#define CLR(x) memset(x,0,_width*_height);

  CLR(_bitmap);
  CLR(_alphaMask);
  memset(_dirty,0,_height);
}

//***********************************************************
// Convert the palette bitmap into yuv + alphamask bitmap
//
//***********************************************************
uint8_t vobSubBitmap::buildYUV( int16_t *palette )
{
  
  uint8_t *ptr;

  ptr=_bitmap;      
        
  for(uint32_t y=0;y<_width*_height;y++)
  {
    *ptr=palette[*ptr];
    ptr++;  
  }
  return 1;
}
//***********************************************************
//
//      Resample the square beginning at position = oldtop with a height of oldheigh
//        to a square of size new, newy
//
//***********************************************************
uint8_t vobSubBitmap::subResize(vobSubBitmap **tgt,uint32_t newx,uint32_t newy,uint32_t oldtop, uint32_t oldheight)
{
  int flags=0;
  SwsContext *ctx=NULL;
  int er=0;
  
  aprintf("Sub Resize : top %lu height %lu -> %lu %lu\n",oldtop, oldheight,newx, newy);
  
#if 0  
//#ifdef USE_MMX
                
#define ADD(x,y) if( CpuCaps::has##x()) flags|=SWS_CPU_CAPS_##y;
                ADD(MMX,MMX);           
                ADD(3DNOW,3DNOW);
                ADD(MMXEXT,MMX2);
#endif  
  flags+=SWS_BILINEAR;
  // Need a new one ?
  // Or reuse the old one ?
  if(*tgt && (*tgt)->_width==newx && (*tgt)->_height==newy)
  {
    (*tgt)->clear(); // useless FIXME
  }
  else
  {
    if(*tgt) delete *tgt;
    *tgt=NULL;
    *tgt=new  vobSubBitmap(newx,newy);    
  }
  
  // Need to resize ?
  if(oldheight==newy && _width==newx)
  {
    uint8_t *src,*dst;
    aprintf("No need to resize\n");
    src=_bitmap+oldtop*_width;
    dst=(*tgt)->_bitmap;
    memcpy(dst,src,newx*newy); 
    
    src=_alphaMask+oldtop*_width;
    dst=(*tgt)->_alphaMask;
    memcpy(dst,src,newx*newy); 
    
    return 1;    
  }
  
  ctx=sws_getContext(
  _width,oldheight,
  IMGFMT_Y8,
  newx,newy,
  IMGFMT_Y8,
  flags,
  NULL, NULL,NULL);

  ADM_assert(ctx);
    
  //************************
  uint8_t *src[3];
  uint8_t *dst[3];
  int ssrc[3];
  int ddst[3];
  

  //resize bitmap
  
  src[0]=_bitmap+oldtop*_width;
  src[1]=NULL;
  src[2]=NULL;

  ssrc[0]=_width;
  ssrc[1]=ssrc[2]=0;

  
  dst[0]=(*tgt)->_bitmap;
  dst[1]=NULL;
  dst[2]=NULL;
  
  ddst[0]=newx;
  ddst[1]=ddst[2]=0;

  er=sws_scale(ctx,src,ssrc,0,oldheight,dst,ddst);
  aprintf("Er:%d\n",er);
  
  // And alpha
  src[0]=_alphaMask+oldtop*_width;  
  dst[0]=(*tgt)->_alphaMask;
  er=sws_scale(ctx,src,ssrc,0,_height,dst,ddst);
  aprintf("Er:%d\n",er);
  // end
  sws_freeContext(ctx); 
  return 1;             
}

//EOF
