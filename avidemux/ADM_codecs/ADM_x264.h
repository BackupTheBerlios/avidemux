//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef USE_X264
#include "ADM_codecs/ADM_x264param.h"

 class X264Encoder     : public encoder
{
  protected :
        uint32_t        _inited;                        
        void            *_handle;
        void            *_pic;
        uint8_t         preamble(uint32_t fps1000,ADM_x264Param *param);
        ADM_x264Param   _param;

  public :
    X264Encoder(uint32_t width,uint32_t height)   : encoder(width,height)
    {        
      _handle=NULL;                             
      _inited=0;
      _pic=NULL;
    } ;
    ~X264Encoder();
                     
    uint8_t         stopEncoder(void );
    virtual uint8_t         init( uint32_t val,uint32_t fps1000,ADM_x264Param *param );
                                
    uint8_t         init(uint32_t a, uint32_t b) {return 0;} // not used
    uint8_t         getResult( void *ress);                
    virtual uint8_t         encode( ADMImage        *in,
                                    uint8_t         *out,
                                    uint32_t        *len,
                                    uint32_t        *flags);
                                

};
//***************** CQ **************** 
   class X264EncoderCQ : public    X264Encoder
{
  protected :
                                
    uint32_t _q;              
        
  public :
    X264EncoderCQ(uint32_t width,uint32_t height) : X264Encoder(width,height)
    {

    } ;
    ~X264EncoderCQ();                    
    virtual         uint8_t init( uint32_t val,uint32_t fps1000,ADM_x264Param *param );     
 
};
//***************** CBR ****************
   class X264EncoderCBR : public    X264Encoder
{
  protected :
                                
    uint32_t _br;              
        
  public :
    X264EncoderCBR(uint32_t width,uint32_t height) : X264Encoder(width,height)
    {

    } ;
    ~X264EncoderCBR();                    
    virtual         uint8_t init( uint32_t br,uint32_t fps1000,ADM_x264Param *param );     
 
};
#endif
