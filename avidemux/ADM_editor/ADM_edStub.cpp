/***************************************************************************
                          ADM_edStub.cpp  -  description
                             -------------------
    begin                : Sat Mar 2 2002
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
#include <assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "ADM_editor/ADM_edit.hxx"

#define STUBB _videos[0]._aviheader


/**
  Convert a frame as seen by the gui to a segment and offset
  in the segment
*/
uint8_t
  ADM_Composer::convFrame2Seg (uint32_t framenum,
			       uint32_t * seg, uint32_t * relframe)
{
  uint32_t curseg = 0, taq = 0;

  while (curseg < _nb_segment)
    {

      if ((framenum >= taq)
	  && (framenum < (taq + _segments[curseg]._nb_frames)))
	{
	  // gotcha
	  *seg = curseg;
	  *relframe = _segments[curseg]._start_frame + framenum - taq;
	  return 1;

	}
      taq += _segments[curseg]._nb_frames;
      curseg++;

    };
  printf("\n Frame 2 seg failed! (%lu)\n",framenum);
  return 0;


}

/**
  Convert a frame inside a segment as a frame as seen by GUI
  */
uint8_t
  ADM_Composer::convSeg2Frame (uint32_t * framenum,
			       uint32_t seg, uint32_t relframe)
{
  uint32_t curseg = 0;
  assert (seg < _nb_segment);
  *framenum = 0;
  while (curseg < seg)
    {

      *framenum += _segments[curseg]._nb_frames;

      curseg++;
    };
#if 0
  printf ("\n Seg: %lu Sum : %lu, relframe :%lu, start:%lu", seg,
	  *framenum, relframe, _segments[seg]._start_frame);
#endif
  *framenum += relframe - _segments[seg]._start_frame;
#if 0
  printf ("--> %lu\n", *framenum);
#endif
  return 1;


}



uint8_t
  ADM_Composer::getFrameNoAlloc (uint32_t framenum, uint8_t * ptr,
				 uint32_t * framelen,
				 uint32_t * flags, uint8_t * seq)
{
  uint32_t relframe;
  uint32_t seg;
  static uint32_t lastseg = 0, lastframe = 0;
  uint32_t ref;

  // convert frame to block, relative frame
  if (!convFrame2Seg (framenum, &seg, &relframe))
    return 0;
#if 0
  printf ("\n %lu --> %lu,%lu\n", framenum, seg, relframe);
#endif
  if (seq)
    {
      if ((lastseg == seg) && ((lastframe + 1) == relframe))
	{
	  *seq = 1;
	}
      else
	*seq = 0;
    }
  lastseg = seg;
  lastframe = relframe;
  ref = _segments[seg]._reference;
  return _videos[ref]._aviheader->getFrameNoAlloc (relframe, ptr,
						   framelen, flags);
}


//
//  For that one the first segment is always right
//                                                                                              
uint32_t ADM_Composer::getTime (uint32_t fn)
{
  return STUBB->getTime (fn);
}

uint32_t ADM_Composer::getFlags (uint32_t frame, uint32_t * flags)
{
  uint32_t
    relframe;
  uint32_t
    seg;
  if (!convFrame2Seg (frame, &seg, &relframe))
    return 0;
  uint32_t
    ref =
    _segments[seg].
    _reference;
  return _videos[ref]._aviheader->getFlags (relframe, flags);
}

uint8_t ADM_Composer::getFrameSize (uint32_t frame, uint32_t * size)
{
  uint32_t
    relframe;
  uint32_t
    seg,
    ref;
  if (!convFrame2Seg (frame, &seg, &relframe))
    return 0;

  ref = _segments[seg]._reference;
  return _videos[ref]._aviheader->getFrameSize (relframe, size);


}


uint8_t ADM_Composer::setFlag (uint32_t frame, uint32_t flags)
{
  uint32_t
    relframe;
  uint32_t
    seg;
  if (!convFrame2Seg (frame, &seg, &relframe))
    return 0;

  uint32_t
    ref =
    _segments[seg].
    _reference;
  return _videos[ref]._aviheader->setFlag (relframe, flags);
}

//
//
uint8_t ADM_Composer::updateVideoInfo (aviInfo * info)
{

  info->nb_frames = _total_frames;
  if (info->fps1000)
    {
      uint32_t
	r,
	s,
	d;
      // we got 1000 * image /s
      // we want rate, scale and duration
      //
      s = 1000;
      r = info->fps1000;
      double
	u;
      u = (double) info->fps1000;
      if (u < 10.)
	u = 10.;
      u = 1000000. / u;
      d = (uint32_t) floor (u);;	// 25 fps hard coded

      //getVideoStreamHeader

      AVIStreamHeader *
	ily =
	_videos[0].
	_aviheader->
	getVideoStreamHeader ();
      ily->dwRate = r;
      ily->dwScale = s;
    }
  return 1;
}

uint8_t ADM_Composer::getVideoInfo (aviInfo * info)
{
  uint8_t
    ret;
  ret = STUBB->getVideoInfo (info);
  if (ret)
    {
      info->nb_frames = _total_frames;
    }
  return ret;
}

//______________________________
//    Info etc... to be removed later
//______________________________                                        
AVIStreamHeader *
ADM_Composer::getVideoStreamHeader (void)
{
  return STUBB->getVideoStreamHeader ();
};
MainAVIHeader *
ADM_Composer::getMainHeader (void)
{
  return STUBB->getMainHeader ();
}

BITMAPINFOHEADER *
ADM_Composer::getBIH (void)
{
  return STUBB->getBIH ();
};
