/***************************************************************************
                          ADM_xvideco.h  -  description
                             -------------------
    begin                : Fri Aug 23 2002
    copyright            : (C) 2002 by mean
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
#ifdef USE_XX_XVID
class decoderXvid:decoders
{
protected:
  void *_handle;


public:
    decoderXvid (uint32_t w, uint32_t h);
    virtual ~ decoderXvid ();
  virtual uint8_t uncompress (uint8_t * in, uint8_t * out, uint32_t len,
			      uint32_t * flag = NULL);
  virtual void setParam (void);
};

#endif
