/***************************************************************************
   
    copyright            : (C) 2006 by mean
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

#ifndef AUDIO_F_FILM2PAL_H
#define AUDIO_F_FILM2PAL_H

#include "ADM_audioFilter.h"
#include "audiofilter_limiter_param.h"
class AUDMAudioFilterFilm2Pal : public AUDMAudioFilter
{
  protected:
    uint32_t   _target;
    uint32_t _removed;
    uint32_t _modulo;
  public:
                          AUDMAudioFilterFilm2Pal(AUDMAudioFilter *previous);
    virtual                ~AUDMAudioFilterFilm2Pal();
    virtual    uint32_t   fill(uint32_t max,float *output,AUD_Status *status);
};
class AUDMAudioFilterPal2Film : public AUDMAudioFilter
{
  protected:
    uint32_t   _target;
    uint32_t _removed;
    uint32_t _modulo;

  public:
                            AUDMAudioFilterPal2Film(AUDMAudioFilter *previous);
    virtual                ~AUDMAudioFilterPal2Film();
    virtual    uint32_t   fill(uint32_t max,float *output,AUD_Status *status);
};

#endif
