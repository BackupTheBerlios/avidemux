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
#define VS_MAXPACKET 128*1024

#define NOSUB 0xffffffff

static FILTER_PARAM vobsubParam={1,{"subname"}};
#define aprintf printf
//*************************************************************
//
typedef struct vobSubParam
{
        char            *subname;
        uint32_t        index;
}vobSubParam;


class  ADMVideoVobSub:public AVDMGenericVideoStream
 {

 protected:
        virtual char                    *printConf(void);
        uint8_t                         readbyte(void);         /// Read a byte from buffer
        uint16_t                        readword(void);         /// Read a 16 bits word from buffer
        uint8_t                         forward(uint32_t v);    /// Read a 16 bits word from buffer
        uint8_t                         decodeRLE(uint32_t off,uint32_t evenline);
        uint8_t                         setup(void);            /// Rebuild internal info
        uint8_t                         cleanup(void);          /// Destroy all internal info
        uint8_t                         paletteYUV( void );     /// Convert RGB Pallette to yuv
        uint8_t                         Palettte2Display( void ); /// Convert the RLE to YUV bitmap
        uint8_t                         handleSub( void );      /// Decode a sub packet
        uint8_t                         buildDisplay( void );   /// Convert palette to yuv bitmap
        uint32_t                        lookupSub(uint64_t time);/// Return sub index corresponding to time
        
        ADM_mpegDemuxerProgramStream    *_parser;
        uint8_t                        *_palettized;            /// bitmap
        uint8_t                         *_data;                 /// Data for packet
        uint8_t                         *_bitmap;               /// YUV image
        uint8_t                         *_alphaMask;               /// alpha mask
        VobSubInfo                      *_vobSubInfo;           /// Info of the index file
        vobSubParam                     *_param;
        
        uint32_t                        _x1,_y1,_x2,_y2;        /// sub boxing
        uint32_t                        _subW,_subH;
        uint8_t                         _displaying;            ///  Is display active
        uint32_t                        _curOffset;
        uint32_t                        _subSize;
        uint32_t                        _dataSize;              /// Size of the data chunk
        
        uint8_t                         _colors[4];             /// Colors palette
        uint8_t                         _alpha[4];              /// Colors alpha
        int16_t                         _YUVPalette[16][3];
        uint32_t                        _currentSub;            ///
 public:
                
                        ADMVideoVobSub(  AVDMGenericVideoStream *in,CONFcouple *setup);
                        ~ADMVideoVobSub();
        virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                                ADMImage *data,uint32_t *flags);
        virtual uint8_t getCoupledConf( CONFcouple **couples)           ;
        virtual uint8_t configure( AVDMGenericVideoStream *instream);
                                                        
};
//*************************************************************
SCRIPT_CREATE(vobsub_script,ADMVideoVobSub,vobsubParam);
BUILD_CREATE(vobsub_create,ADMVideoVobSub);
//*************************************************************
uint8_t ADMVideoVobSub::configure(AVDMGenericVideoStream *in)
{
char *name;
       GUI_FileSelRead("Select .sub file to load, NOT .idx", &name);        
       if(name)
       {
                cleanup();
                if(_param->subname) ADM_dealloc(_param->subname);
                _param->subname=name;
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
        _palettized=NULL;   
        
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
                _param->index = 0x20;                
        }
        
        setup();
}
//************************************
uint8_t ADMVideoVobSub::setup(void)
{
   _vobSubInfo=NULL;
   if(_param->subname)
        {
                printf("Opening %s\n",_param->subname);
                if(vobSubRead(_param->subname,&_vobSubInfo))
                {
                        printf("Opening index \n");
                        _parser=new ADM_mpegDemuxerProgramStream(_param->index,0xe0);
                        if(!_parser->open(_param->subname))
                        {
                                printf("Mpeg Parser : opening %s failed\n",_param->subname);
                                delete _parser;
                                _parser=NULL;
                
                         }
                }
        }
        
        
        if(!_parser)
        {
                printf("opening of vobsub file failed\n");
        }
      
        _x1=_y1=_x2=_y2=0;
        _data=new uint8_t [VS_MAXPACKET];
        _subSize=0;
        _subW=_subH=0;
        
        _bitmap=NULL;
        _alphaMask=NULL;
        _currentSub=NOSUB;
                

}
//*************************************************************
uint8_t ADMVideoVobSub::cleanup(void)
{

        if(_parser) delete _parser;
        _parser=NULL;
        if(_palettized) delete [] _palettized;
        _palettized=NULL;
        if(_data) delete [] _data;
        _data=NULL;
        if(_vobSubInfo) destroySubInfo( _vobSubInfo);
        _vobSubInfo=NULL;
        if(_bitmap) delete [] _bitmap;
        _bitmap=NULL;
        
       if(_alphaMask) delete [] _alphaMask;
        _alphaMask=NULL;
        

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
                printf("No matching sub for time %llu frame%lu\n",time,frame);
                return 1;
        }
        if(sub!=_currentSub )
        {                
                 _parser->_asyncJump2(0,_vobSubInfo->lines[sub].fileOffset);
                 handleSub();
                _currentSub=sub;
                
        }                
        
        // Next one
        if(_palettized)
        {
                Palettte2Display(); // Create the bitmap
                
                // Merge
                
                uint32_t stridein,strideout,len;
                uint8_t *in,*out,*mask,*in2;
                uint16_t old,nw,alp;
                
                stridein=_x2-_x1;
                strideout=_info.width;
                
                if(strideout>stridein) len=stridein;
                                else    len=strideout;
                                
                in=_bitmap;
                mask=_alphaMask;
               
                out=data->data+_y1*strideout+_x1;
                
                for(uint32_t y=_y1;y<_y2;y++)
                {
                        for(uint32_t x=_x1;x<_x2;x++)
                        {
                               old=out[x];
                               nw=in[x];
                               alp=mask[x];
#if 0                               
#define ACOEF alp
#define BCOEF (15-alp)                               
#else
#define BCOEF alp
#define ACOEF (15-alp)                               
#endif
                               nw=old*(ACOEF)+(BCOEF)*nw;
                               out[x]=nw>>4;
                        }
                        //memcpy(out,in,len);
                        out+=strideout;
                        in+=stridein;
                        mask+=stridein;
                }
        }
        return 1;
}
//***********************************************        

uint8_t  ADMVideoVobSub::handleSub( void )
{       
uint16_t date,next,dum;
uint8_t  command;
        _subSize=0;

        // Read data
        _subSize=_parser->read16i();
        ADM_assert(_subSize);
        
        aprintf("Vobsub: data len =%d\n",_subSize);
        
        ADM_assert(_subSize<VS_MAXPACKET);
        _parser->read(_data+2,_subSize-2);
       
        // We got the full packet
        // now scan it
        _curOffset=2;
        _dataSize=readword();
        aprintf("data block=%lu\n",_dataSize);
        ADM_assert(_dataSize>4);
        forward(_dataSize-4);    // go to the command block
        
        
        while(1)
        {
             
                date=readword();
                next=readword();
                if(next==_curOffset-4) break;            // end of command
                while(_curOffset<next)
                {
                        command=readbyte();
                        aprintf("vobsub:Command : %d date:%d next:%d cur:%lu\n",command,date,next,_curOffset);
                        switch(command)
                        {
                                case 0: _displaying=1;
                                        break;
                                case 01: // start date
                                        break;
                                case 02: // stop date
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
                                                a=readword();
                                                b=readword();
                                                c=readword();
                                                _x1=a>>4;
                                                _x2=((a&0xf)<<8)+(b>>8);
                                                _y1=((b&0xf)<<4)+(c>>12);
                                                _y2=c&0xfff;
                                                aprintf("vobsuv: x1:%d x2:%d y1:%d y2:%d\n",_x1,_x2,_y1,_y2);
                                                if(_palettized) delete [] _palettized;
                                                _subW=_x2-_x1;
                                                _subH=_y2-_y1;
                                                
                                                _palettized=new uint8_t [_subW*_subH];
                                                _bitmap=new uint8_t [_subW*_subH];
                                                _alphaMask=new uint8_t [_subW*_subH];
                                                
                                                memset(_palettized,0,_subW*_subH);
                                                memset(_bitmap,0,_subW*_subH);
                                                memset(_alphaMask,0,_subW*_subH);
                                                                        
                                        }
                                        break;
                                case 06: // RLE offset 
                                        // 2*16 bits : odd offset, even offset
                                        {
                                        uint32_t odd,even;
                                        odd=readword();
                                        even=readword();
                                        decodeRLE(odd,0);
                                        decodeRLE(even,1);
                                        }
                                        break;                
                        } //End switch command     
                }// end while
        }
     
}
//*********************************************************
// Convert the palette display into YUV bitmap
//*********************************************************
uint8_t ADMVideoVobSub::buildDisplay( void )
{

uint8_t *ptrin, *ptrout,*maskout;

int     color;
int     alpha;
int     old,nw,cur;

        ADM_assert(_bitmap);
        ADM_assert(_palettized);

        ptrin=_palettized;
        ptrout=_bitmap;
        maskout=_alphaMask;
        
        
        for(uint32_t y=0;y<_subH;y++)
        for(uint32_t x=0;x<_subW;x++)
        {
                // Alpha blend it
                
                color=*ptrin;
                alpha=_alpha[color];
                nw=_YUVPalette[color][0];
                
                *ptrout=nw;
                *maskout=alpha;
        
                
                ptrin++;
                ptrout++;
                maskout++;
        }
        return 1;
}
//***********************************************************
uint8_t ADMVideoVobSub::Palettte2Display( void )
{
        ADM_assert(_parser);
        ADM_assert(_vobSubInfo);
        // First convert all the palette from RGB to YUV
        paletteYUV();
        // Then Process the RLE Datas
        // To get the _bitmap yuv data
        buildDisplay();

        return 1;
}
//***********************************************************
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
               
                _YUVPalette[i][0]=y;
                _YUVPalette[i][1]=u;
                _YUVPalette[i][2]=v;
        
        }
        return 1;

}
//***********************************************************
// RLE code inspired from mplayer
uint8_t ADMVideoVobSub::decodeRLE(uint32_t off,uint32_t start)
{
        uint32_t oldoffset=_curOffset;
        uint32_t stride=_subW;
        uint32_t x,y;
        uint8_t *ptr=_palettized;
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
        while(_curOffset<_dataSize && y<(_subH>>1))
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
                                if(a<4)
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
              
              memset(ptr,color,run);
              x+=run;
              ptr+=run;
              //  aprintf("x:%d y:%d\n",x,y);
              if(x>=stride)
              {
                        
                     y++;
                     x=0;
                     ptr=_palettized+(y*2+start)*stride;
                     SKIPNIBBLE;
              }
        }
        aprintf("vobsub End :%d y:%d\n",_curOffset,y); 
        _curOffset=oldoffset;
} 
//
//      Return the index in the sub table of the sub matching the time
//
uint32_t ADMVideoVobSub::lookupSub(uint64_t time)
{
int32_t i;
        i=_vobSubInfo->nbLines-2;
        while(i>=0)
        {
                if(_vobSubInfo->lines[i].startTime<=time && _vobSubInfo->lines[i+1].startTime>time)
                {
                        printf("Matching for time %llu : sub %lu starting at :%lu\n",
                                        time,i,_vobSubInfo->lines[i].startTime);
                        return i; 
                }                        
                i--;       
        }
        return NOSUB;


}
//EOF

