 
/***************************************************************************
    copyright            : (C) 2002-6 by mean
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
#include <math.h>

#include <ADM_assert.h>

#include "avifmt.h"
#include "avifmt2.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"

AUDMEncoder::AUDMEncoder(AUDMAudioFilter *in)  :AVDMBufferedAudioStream  (NULL)
{
  _wavheader = new WAVHeader;
  _incoming=in;
  memcpy(_wavheader, _incoming->getInfo(), sizeof(WAVHeader));
  _wavheader->encoding=WAV_AAC;
  _incoming->rewind();	// rewind
  _handle=NULL;
  _extraData=NULL;
  _extraSize=0;
};
AUDMEncoder::~AUDMEncoder()
{
  cleanup();
};

uint8_t AUDMEncoder::cleanup(void)
{
  if(_wavheader) delete(_wavheader);
  _wavheader=NULL;

  if(_extraData) delete [] _extraData;
  _extraData=NULL;
};

