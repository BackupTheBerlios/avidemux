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
//


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


#define VOBSUB "/capture/sub/phone.sub"
#define VS_MAXPACKET 128*1024

static FILTER_PARAM vobsubParam={0,{""}};
#define aprintf printf
//*************************************************************
class  ADMVideoVobSub:public AVDMGenericVideoStream
 {

 protected:
        virtual char                    *printConf(void);
        ADM_mpegDemuxerProgramStream    *_parser;
        uint32_t                        *_display;              /// bitmap
        uint8_t                         *_data;                 /// Data for packet
        uint32_t                        _x1,_y1,_x2,_y2;        /// sub boxing
        uint8_t                         _displaying;            ///  Is display active
        uint32_t                        _curOffset;
        uint8_t                         readbyte(void);         /// Read a byte from buffer
        uint16_t                        readword(void);         /// Read a 16 bits word from buffer
        uint8_t                         forward(uint32_t v);    /// Read a 16 bits word from buffer
        uint32_t                        _subSize;
        
 public:
                
                        ADMVideoVobSub(  AVDMGenericVideoStream *in,CONFcouple *setup);
                        ADMVideoVobSub();
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
        
        return 1;
        
}
 
//*************************************************************
char *ADMVideoVobSub::printConf( void )
{
        static char buf[50];
        
        sprintf((char *)buf," VobSub");
        return buf;
}
//*************************************************************
ADMVideoVobSub::ADMVideoVobSub(  AVDMGenericVideoStream *in,CONFcouple *setup)
{
    UNUSED_ARG(setup);
        _in=in;         
        memcpy(&_info,_in->getInfo(),sizeof(_info));    
        _info.encoding=1;       
        _uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
        ADM_assert(_uncompressed); 
        _parser=NULL;  
        _display=NULL;   
        
        _parser=new ADM_mpegDemuxerProgramStream(0x20,0xe0);
        if(!_parser->open(VOBSUB))
        {
                delete _parser;
                _parser=NULL;
                printf("opening of vobsub file failed\n");
        }
        _x1=_y1=_x2=_y2=0;
        _data=new uint8_t [VS_MAXPACKET];
        _subSize=0;
                
}
//*************************************************************
ADMVideoVobSub::ADMVideoVobSub()
{
        delete  _uncompressed;  
        _uncompressed=NULL;
        if(_parser) delete _parser;
        _parser=NULL;
        if(_display) delete [] _display;
        _display=NULL;
        if(_data) delete [] _data;
        _data=NULL;
  
}
uint8_t ADMVideoVobSub::forward(uint32_t v)
{
        aprintf("Current : %lu forwarding : %lu\n",_curOffset,v);
         ADM_assert(_curOffset+v<_subSize);
         _curOffset+=v;
         return 1;

}
uint8_t ADMVideoVobSub::readbyte( void )
{
        ADM_assert(_curOffset<_subSize);
        return _data[_curOffset++];

}
uint16_t ADMVideoVobSub::readword( void )
{
uint16_t w;
        ADM_assert(_curOffset<_subSize-1);
        w= _data[_curOffset]<<8;
        w+= _data[_curOffset+1];
        _curOffset+=2;
        return w;
}

uint8_t ADMVideoVobSub::getCoupledConf( CONFcouple **couples)
{
        return 0;
}
//*************************************************************
uint8_t ADMVideoVobSub::getFrameNumberNoAlloc(uint32_t frame,
                                uint32_t *len,
                                ADMImage *data,
                                uint32_t *flags)
{

uint16_t date,next,datasize;
uint8_t  command;
        _subSize=0;

        ADM_assert(frame<_info.nb_frames);
        // read uncompressed frame
        if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

        // Read data
        _subSize=_parser->read16i();
        ADM_assert(_subSize);
        
        aprintf("Vobsub: data len =%d\n",_subSize);
        
        ADM_assert(_subSize<VS_MAXPACKET);
        _parser->read(_data+2,_subSize-2);
       
        // We got the full packet
        // now scan it
        _curOffset=2;
        datasize=readword();
        aprintf("data block=%lu\n",datasize);
        ADM_assert(datasize>4);
        forward(datasize-4);    // go to the command block
        
        
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
                                        readword();
                                        break;          
                                case 0xff:
                                        break;
                                case 04: // alpha channel
                                         //4 nibble= 16 bits
                                        readword();
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
                                                                        
                                        }
                                        break;
                                case 06: // RLE offset 
                                        // 2*16 bits : odd offset, even offset
                                        readword();
                                        break;                
                        } //End switch command     
                }// end while
        }
        data->copyInfo(_uncompressed);
        return 1;
}



//EOF

