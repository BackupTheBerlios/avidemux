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

class X264Encoder:public encoder
{
protected:uint32_t _inited;
  void *_handle;
  void *_pic;
  uint8_t 	*_seiUserData;
  uint32_t  _seiUserDataLen; // X264 user data
  uint8_t preamble (uint32_t fps1000, ADM_x264Param * param);
  ADM_x264Param _param;
  uint8_t *extraData;
  uint32_t extraSize;
  uint8_t createHeader (void);
  uint32_t ptsFrame;
  uint32_t curFrame;
public:  X264Encoder (uint32_t width, uint32_t height):encoder (width,
							   height)
  {
    _handle = NULL;
    _inited = 0;
    _pic = NULL;
    extraData = NULL;
    extraSize = 0;
    ptsFrame = 0;
    curFrame = 0;
    _seiUserDataLen=0;
    _seiUserData=NULL;
    
  };
  ~X264Encoder ();

  uint8_t stopEncoder (void);
  virtual uint8_t init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * param);

  uint8_t init (uint32_t a, uint32_t b)
  {
    return 0;
  }				// not used
  virtual uint8_t encode (ADMImage * in, ADMBitstream * out);
  virtual uint8_t getExtraData (uint32_t * l, uint8_t ** d);

};
//***************** CQ **************** 
class X264EncoderCQ:public X264Encoder
{
protected:uint32_t _q;

public:X264EncoderCQ (uint32_t width, uint32_t height):X264Encoder (width,
							       height)
  {

  };
  ~X264EncoderCQ ();
  virtual uint8_t init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * param);

};
//***************** AQ **************** 
class X264EncoderAQ:public X264Encoder
{
protected:uint32_t _q;

public:X264EncoderAQ (uint32_t width, uint32_t height):X264Encoder (width,
							       height)
  {

  };
  ~X264EncoderAQ ();
  virtual uint8_t init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * param);

};
//***************** CBR ****************
class X264EncoderCBR:public X264Encoder
{
protected:uint32_t _br;

public:X264EncoderCBR (uint32_t width, uint32_t height):X264Encoder (width,
								height)
  {

  };
  ~X264EncoderCBR ();
  virtual uint8_t init (uint32_t br, uint32_t fps1000, ADM_x264Param * param);

};
//***************** Pass1 **************** 
class X264EncoderPass1:public X264Encoder
{
protected:uint32_t _q;

public:X264EncoderPass1 (uint32_t width,
		    uint32_t height):X264Encoder (width, height)
  {

  };
  ~X264EncoderPass1 ();
  virtual uint8_t init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * param);

};
//***************** Pass 2 **************** 
class X264EncoderPass2:public X264Encoder
{
protected:uint32_t _q;

public:X264EncoderPass2 (uint32_t width,
		    uint32_t height):X264Encoder (width, height)
  {

  };
  ~X264EncoderPass2 ();
  virtual uint8_t init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * param);

};
#endif
