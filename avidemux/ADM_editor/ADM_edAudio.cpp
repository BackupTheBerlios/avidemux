/***************************************************************************
                          ADM_edAudio.cpp  -  description

This class is a relay class to edit embbeded audio
It does nothing really important except defining an interface
to generic audio stream


                             -------------------
    begin                : Wed Mar 13 2002
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stream.h>
#include <assert.h>
#include <math.h>
#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "indexer.hxx"
//#include "aviaudio.hxx"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_editor/ADM_edAudio.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_EDITOR
#include "ADM_toolkit/ADM_debug.h"


extern ADM_Composer *video_body;

// Build information
//

AVDMEditAudioStream::AVDMEditAudioStream (ADM_Composer * father)
{
  uint32_t l = 0;
  uint8_t *d = NULL;

  _father = father;
  _destroyable = 0;
  _pos = 0;
  strcpy (_name, "FILE:AVIED");
  printf ("\n Editor :Audio streamer initialized");
  // computing size & info
  _length = video_body->getAudioLength ();
  _wavheader = video_body->getAudioInfo ();
  video_body->getAudioExtra (&l, &d);
  if (_wavheader)
    {
      _codec = getAudioCodec (_wavheader->encoding, _wavheader, l, d);
    }
  else
    _codec = getAudioCodec (0);
  assert (_codec);
  _vbr = 0;
}

// Seek....

uint8_t AVDMEditAudioStream::goTo (uint32_t newoffset)
{
 aprintf("Editor audio : Goto %lu\n",newoffset);
  if (video_body->audioGoTo (newoffset))
    {
      _pos = newoffset;
      return 1;
    }

  return 0;
}

uint8_t AVDMEditAudioStream::buildAudioTimeLine (void)
{
  // we assume there is only one file
//        AVDMGenericAudioStream::buildAudioTimeLine();
  // now warn father to build the map for childs
  _father->propagateBuildMap ();
  _vbr = 1;
  return 0;
}

uint8_t AVDMEditAudioStream::goToTime (uint32_t mstime)
{
aprintf("Editor audio : Gototime  %lu\n",mstime);
  return video_body->audioGoToTime (mstime, &_pos);
}

uint32_t
  AVDMEditAudioStream::readPCMeq (uint32_t lenasked,
				  uint8_t * out, uint32_t * lenout)
{
  // we will have to ask each part to the father 
  // and keep sync
  uint32_t got;
  assert (_vbr);

  got = video_body->readPCMeq (lenasked, out, lenout);
  _pos += got;
  return got;

}

uint32_t AVDMEditAudioStream::read (uint32_t len, uint8_t * buffer)
{


  int32_t
    done =
    len,
    ck;
  do
    {
      ck = video_body->audioRead (done, buffer);
      // printf("\n\t read : asked : %lu %lu",done,ck);
      if (!ck)
	return len - done;

	// case of vorbis
	#warning GROSS HACK
		if(_wavheader->encoding==WAV_OGG) return ck;
	
	#warning /GROSS HACK	
      done -= ck;
      buffer += ck;
      _pos += ck;
    }
  while (done > 0);
  return len - done;

}
