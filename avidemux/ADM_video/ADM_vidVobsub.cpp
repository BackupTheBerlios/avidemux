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
#include <math.h>
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

#define VOBSUB "/capture/sub/phone.sub"

#include "ADM_vidVobSub.h"

extern uint8_t DIA_vobsub(char **name, uint32_t *idx);

static FILTER_PARAM vobsubParam={1,{"subname"}};
#define aprintf printf
//*************************************************************
//

//*************************************************************
SCRIPT_CREATE(vobsub_script,ADMVideoVobSub,vobsubParam);
BUILD_CREATE(vobsub_create,ADMVideoVobSub);
//*************************************************************
uint8_t ADMVideoVobSub::configure(AVDMGenericVideoStream *in)
{

       
  if(DIA_vobsub(&(_param->subname),&(_param->index)))
       {
                cleanup();
                setup();
                return 1;
       }
        
        return 0;        
}
 
//*************************************************************
char *ADMVideoVobSub::printConf( void )
{
        static char buf[50];
        
        sprintf((char *)buf," VobSub");
        return buf;
}
//*************************************************************
ADMVideoVobSub::ADMVideoVobSub(  AVDMGenericVideoStream *in,CONFcouple *couples)
{

        _in=in;         
        memcpy(&_info,_in->getInfo(),sizeof(_info));    
        _info.encoding=1;       
        _parser=NULL;  
        _resampled=NULL;
        _chromaResampled=NULL;
        _original=NULL;        
        
        _param=NEW(vobSubParam);
        
        if(couples)
        {                 
                GET(subname);
                GET(index);                               
        }
        else
        {
#ifdef KK_ADM_DEBUG
                _param->subname=ADM_strdup(VOBSUB);
#else                
                _param->subname =NULL;
#endif                
                _param->index = 0;                
        }
        
        setup();
}
//************************************
uint8_t ADMVideoVobSub::setup(void)
{
  char *dup;
  int l;
  
   _vobSubInfo=NULL;
   if(_param->subname && strlen(_param->subname)>5)
        {
                printf("Opening %s\n",_param->subname);
                dup=ADM_strdup(_param->subname);
                l=strlen(dup);
                if(l>5)
                  if(dup[l-4]=='.')
                {
                  dup[l-3]='s';
                  dup[l-2]='u';
                  dup[l-1]='b';
                          
                }
                if(vobSubRead(_param->subname,_param->index,&_vobSubInfo))
                {
                        printf("Opening index \n");
                       
                        
                        _parser=new ADM_mpegDemuxerProgramStream(_param->index+0x20,0xe0);
                        
                        if(!_parser->open(dup))
                        {
                                printf("Mpeg Parser : opening %s failed\n",_param->subname);
                                delete _parser;
                                _parser=NULL;
                
                         }
                         
                }
                ADM_dealloc(dup);
        }
        
        
        if(!_parser)
        {
                printf("opening of vobsub file failed\n");
        }
         else
        {       // Recompute sub duration
                uint32_t end;
                vobSubLine *cur,*next;
                // Assuming max displat time = MAX_DISPLAY_TIME
                for(uint32_t i=0;i<_vobSubInfo->nbLines-1;i++)
                {
                        if(i && !_vobSubInfo->lines[i].startTime)
                        {
                                _vobSubInfo->lines[i].startTime=0xf0000000;
                                _vobSubInfo->lines[i].stopTime=0xf0000001;
                                
                        }
                        else
                        {
                                cur=&_vobSubInfo->lines[i];
                                next=&_vobSubInfo->lines[i+1];
                        
                                end=cur->startTime+MAX_DISPLAY_TIME;
                                if(end>=next->startTime) end=next->startTime-1;
                                cur->stopTime=end;                                
                        }
                }
                _vobSubInfo->lines[_vobSubInfo->nbLines-1].stopTime=
                        MAX_DISPLAY_TIME+_vobSubInfo->lines[_vobSubInfo->nbLines-1].startTime;
                // Convert all the palette from RGB to YUV
                paletteYUV();
        }
      
        _x1=_y1=_x2=_y2=0;
        _data=new uint8_t [VS_MAXPACKET];
        _subSize=0;
        _subW=_subH=0;
        
        memset(&_original,0,sizeof(_original));
        _currentSub=NOSUB;
        _initialPts=0;
                

}
//*************************************************************
uint8_t ADMVideoVobSub::cleanup(void)
{

        if(_parser) delete _parser;
        _parser=NULL;
        
        if(_original)
          delete _original;
        _original=NULL;        
        
        if(_resampled)
          delete _resampled;
        _resampled=NULL;
        
        if(_chromaResampled)
          delete _chromaResampled;
        _chromaResampled=NULL;
        
        
        if(_data) delete [] _data;
        _data=NULL;
        
        if(_vobSubInfo) destroySubInfo( _vobSubInfo);
        _vobSubInfo=NULL;
        

}
//*************************************************************
ADMVideoVobSub::~ADMVideoVobSub()
{
        cleanup();
         if(_param)
        {
                if(_param->subname)  ADM_dealloc(_param->subname);
                DELETE(_param);
        }
        _param=NULL;
}

//*************************************************************
uint8_t ADMVideoVobSub::getCoupledConf( CONFcouple **couples)
{
                        ADM_assert(_param);
                        *couples=new CONFcouple(2);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
                        CSET(subname);
                        CSET(index);
                        

                        return 1;
}
//*************************************************************
uint8_t ADMVideoVobSub::getFrameNumberNoAlloc(uint32_t frame,
                                uint32_t *len,
                                ADMImage *data,
                                uint32_t *flags)
{

uint64_t time;
uint32_t sub;

        ADM_assert(frame<_info.nb_frames);
        // read uncompressed frame
        if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

        if(!_parser)        
        {
                //
                printf("No valid vobsub to process\n");
                
                return 1;
        
        }
        
        time=(frame+_info.orgFrame);
        time=(time*1000*1000)/_info.fps1000;
        
        sub=lookupSub(time);  
        // Should we re-use the current one ? 
        if(sub==NOSUB)
        {
                aprintf("No matching sub for time %llu frame%lu\n",time,frame);
                return 1;
        }
        // If it is a new sub, decode it...
        if(sub!=_currentSub )
        {                
                 _parser->_asyncJump2(0,_vobSubInfo->lines[sub].fileOffset);
                 _initialPts=_parser->getPTS();
                 handleSub(sub);
                _currentSub=sub;
                Palettte2Display(); // Create the bitmap
                // Time to resize the bitmap
                // First try : Do it bluntly
                
        }                
        
        // and if there is something to display, display it
        //
        vobSubBitmap *src;
        //src=_original;
        src=_resampled;
        
        if(src)          
        {
               
          aprintf("We have %lu %lu to merge\n",src->_width,src->_height);
                // Merge
                
                uint32_t stridein,strideout,len;
                uint8_t *in,*out,*mask,*in2;
                uint16_t old,nw,alp;
                uint32_t xx,yy;
                
                stridein=src->_width;
                strideout=_info.width;
                
                if(strideout>stridein)
                {
                        len=stridein;
                        xx=src->_width;
                }
                else
                {
                        xx=_info.width;
                        len=strideout;
                }
                if(src->_height>_info.height) yy=_info.height;
                        else                 yy=src->_height;           
                in=src->_bitmap;
                mask=src->_alphaMask;
               
                out=data->data+_info.width*src->placeTop;
                // auto center
                uint32_t center=_info.width-src->_width;
                out+=(center>>2)*2;
                
                for(uint32_t y=0;y<yy;y++)
                {
                  for(uint32_t x=0;x<xx;x++)
                        {
                               old=out[x];
                               nw=in[x];
                               alp=mask[x];

                                if(alp) 
                                {
                                  if(alp>7)  nw=old*(16-alp-1)+(alp+1)*nw;
                                        else nw=old*(16-alp)+(alp)*nw;
                                  out[x]=nw>>4;                                         
                                }

                               //out[x]=nw;
                        }
                        //memcpy(out,in,len);
                        out+=strideout;
                        in+=stridein;
                        mask+=stridein;
                }
                
                // Now do chroma u & chroma V
#define DOCHROMA                
#if defined(DOCHROMA)                
                uint32_t crosspage=(_info.width*_info.height)>>2;
                
                strideout=_info.width>>1;
                stridein=_chromaResampled->_width;
                
                out=data->data+_info.width*_info.height;
                out+=(src->placeTop>>1)*(_info.width>>1);
                mask=_chromaResampled->_alphaMask;
                // Center
                out+=(center>>2)*1;
                if(strideout>stridein) xx=stridein;
                else            xx=strideout;
                
                int16_t left=(_info.height>>1)-(_chromaResampled->_height+(_original->placeTop>>1));
                
                if(left<_chromaResampled->_height) yy=left;
                else yy=_chromaResampled->_height;
                
                for(uint32_t y=0;y<yy;y++)
                {
                   for(uint32_t x=0;x<xx;x++)
                   {
                        if(mask[x]>10)         
                        {
                          int16_t val;
                          val=out[x];
                          val-=128;
                         
                          
                          
                          nw=val*(16-alp);
                          
                          val/=4;
                          val=val+128;
                                
                          out[x]=val; 
                          out[crosspage+x]=val;
                        }                                                 
                   }
                   out+=strideout;
                   mask+=stridein;
                }
#endif                
        }
        return 1;
}
//*************************************************************************
//
//      Convert the original bitmap to a rescaled & repositionned one
//      that will be blended into the current picutr
//
//*************************************************************************
uint8_t ADMVideoVobSub::Palettte2Display( void )
{
        ADM_assert(_parser);
        ADM_assert(_vobSubInfo);
     
        // Then Process the RLE Datas
        // To get the _bitmap yuv data
        ADM_assert(_original);
        
        // Set correct color
        _original->buildYUV(_YUVPalette);
        
        // rebuild the scaled one
        // Compute the target size
        uint32_t fx,fy;
        uint32_t ox,oy;
        uint32_t sx,sy;
        /*
                Fx, fy : Final size of the image (i.e size of the current picture)
                ox,oy  : Original size of the image where the sub is coming from
                sx,sy  : Size of the  sub
        
                And we want the final size of the sub
                        + coordinates but that we will do later
        
        */
        
        fx=_info.width;
        fy=_info.height;
        
        ox=_vobSubInfo->width;
        oy=_vobSubInfo->height;
        
        sx=_subW;
        sy=_subH;
        
        // Search the 1st/last non null line
        uint32_t top=0,bottom=0;
        while(top<oy && !_original->_dirty[top]) top++;
        
        bottom=_original->_height-1;
        while(bottom && !_original->_dirty[bottom]) bottom--;
        
        // If true it means we have 2 subs, one on top, one on bottom
        //
        if(bottom>(oy>>1) && top<(oy>>1) && (bottom-top>(oy>>1)))
        {
          // in that case, take only the lower one
          top=oy>>1;
          while(top<oy && !_original->_dirty[top]) top++;                    
        }
        //
        //  The useful part is between top & bottom lines
        //
        
        // Shrink factor
        // The shrink factor is the one used to shrink from the original video
        // to the resize video
        
        double scale,l;
        scale=fx;
        scale/=ox;
        printf("top %lu : bottom :%lu Scale :%f ox:%lu oy:%lu fx:%lu \n",top,bottom,scale,ox,oy,fx);
        
        // We rescale the sub by the same factor
        // Only the visible / useful part
        l=scale;
        l=l*sx;
        sx=(uint32_t )floor(l);
        
       
        l=scale;
        l=l*(bottom-top);
        sy=(uint32_t )floor(l);
        

        // And we resize that useful part of sub
        // to our final bitmap (resampled)
                
        _original->subResize(&_resampled,sx,sy,top, bottom-top);
                
        uint32_t tail;
        
        // Set the position of the sub so that it is ok
        
        tail=16+sy;
        
        if(tail>fy) tail=0;
        else
        {
            tail=fy-tail;           
        }
        
        _resampled->placeTop=tail;
        
        _resampled->subResize(&_chromaResampled,sx>>1,sy>>1,0,sy);
        return 1;
}

//
//      Return the index in the sub table of the sub matching the time
//
uint32_t ADMVideoVobSub::lookupSub(uint64_t time)
{
int32_t i;
        i=0;
        while(i<_vobSubInfo->nbLines-1)
        {
                if(_vobSubInfo->lines[i].startTime<=time && _vobSubInfo->lines[i].stopTime>time)
                {
                        aprintf("Matching for time %llu : sub %lu starting at :%lu\n",
                                        time,i,_vobSubInfo->lines[i].startTime);
                        return i; 
                }   
                if(   _vobSubInfo->lines[i].startTime > time) return NOSUB;                   
                i++;       
        }
        return NOSUB;


}
//EOF

