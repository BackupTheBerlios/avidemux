
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _VOBSUB_V_
#define _VOBSUB_V_

#define VS_MAXPACKET 128*1024

#define NOSUB 0xffffffff

#define MAX_DISPLAY_TIME 3000 // 3s

//************************************
typedef struct vobSubParam
{
        char            *subname;
        uint32_t        index;
}vobSubParam;
//************************************
class vobSubBitmap
{
  public:
        uint32_t                        _width,_height;
        uint32_t                        placeTop, placeHeight;     /// Position of the sub
      
        uint8_t                         *_bitmap;                  /// YUV image
        uint8_t                         *_alphaMask;               /// alpha mask 
        uint8_t                         *_dirty;                   /// Dirty lines (non transparent)
 
                                        vobSubBitmap(uint32_t w, uint32_t h); 
                                        ~vobSubBitmap();
        void                            clear(void);
        
                                        /// Convert palette bitmap to yuv&mask bitmap
        uint8_t                         buildYUV( int16_t *palette ); 
                                        /// Generate the final bitmap (resized)
        uint8_t                         subResize(vobSubBitmap **tgt,uint32_t newx,uint32_t newy,
                                                uint32_t oldtop, uint32_t oldheight);
};
//************************************
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
        uint8_t                         handleSub( uint32_t idx );/// Decode a sub packet
        uint32_t                        lookupSub(uint64_t time);/// Return sub index corresponding to time
        
        ADM_mpegDemuxerProgramStream    *_parser;        
        uint8_t                         *_data;                 /// Data for packet
        VobSubInfo                      *_vobSubInfo;           /// Info of the index file
        vobSubParam                     *_param;
        vobSubBitmap                    *_original;              /// True size (..) depacked vobsub
        vobSubBitmap                    *_resampled;            /// Final one; to be blended in picture
        vobSubBitmap                    *_chromaResampled;      /// Same as above but shinked by 2
        uint32_t                        _x1,_y1,_x2,_y2;        /// sub boxing
        uint32_t                        _subW,_subH;
        uint8_t                         _displaying;            ///  Is display active
        uint32_t                        _curOffset;
        uint32_t                        _subSize;
        uint32_t                        _dataSize;              /// Size of the data chunk
        
        uint8_t                         _colors[4];             /// Colors palette
        uint8_t                         _alpha[4];              /// Colors alpha
        int16_t                         _YUVPalette[16];        /// Luma only
        uint32_t                        _currentSub;            ///
        uint32_t                        _initialPts;
 public:
                
                        ADMVideoVobSub(  AVDMGenericVideoStream *in,CONFcouple *setup);
                        ~ADMVideoVobSub();
        virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                                ADMImage *data,uint32_t *flags);
        virtual uint8_t getCoupledConf( CONFcouple **couples)           ;
        virtual uint8_t configure( AVDMGenericVideoStream *instream);
                                                        
};
#endif
