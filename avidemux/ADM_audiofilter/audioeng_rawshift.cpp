/***************************************************************************
                          audioeng_rawshift.cpp  -  description
                             -------------------

  This filter is similar to timeshift but work also on compressed stream
  (i.e. no need to recompress)                             	


    begin                : Fri Jun 28 2002
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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
// Ctor       `AVDMProcessAudioStream::AVDMProcessAudioStream(AVD
//__________

AVDMProcessAudio_RawShift::AVDMProcessAudio_RawShift(
AVDMGenericAudioStream * instream, int32_t msoff,uint32_t starttime):
AVDMProcessAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));

    // first go to the beginiing....
    _timeoffset=starttime;
    _instream->goToTime(  _timeoffset);
	_startoffset=_instream->getPos();
	 printf("\n** start raw :%lu ",_startoffset);

 	msoff=-msoff;
    _outpos=0;

    if (msoff > 0)
    {
      _instream->goToTime(  _timeoffset+msoff);
	  _byteoffset =_instream->getPos()-_startoffset;
  	}
    else
    {

       printf("\n** start sync :%lu ",_instream->getPos());
       _instream->goToTime( _timeoffset-msoff);
       printf("\n**  end sync :%lu ",_instream->getPos());

		_byteoffset =_instream->getPos()-_startoffset;  		
  		printf("**** Dup byte offset : %ld",_byteoffset);
		_byteoffset=-_byteoffset;
  	  _instream->goToTime(  _timeoffset); 	
        _outpos=-_byteoffset;

  	}


    strcpy(_name, "PROC:RAWS");
    _length = instream->getLength();
    printf("\n Raw shit : %ld ms, byteoffset = %ld\n",msoff,_byteoffset);


};


uint32_t AVDMProcessAudio_RawShift::readDecompress(uint32_t len,
						    uint8_t * buffer)
{

    uint32_t pos=0;


    // no more offset to apply
    if (_outpos == 0)
			return _instream->read(len, buffer);

 	 // still in prebuffer
   if(len<_outpos)
   	{
      	
	   	pos= _instream->read(len, buffer);
     	_outpos-=pos;
      printf("\n prebuffering : %lu",_outpos);
      	return pos;
     }
    // need mode or 0
    // first empty prebuffer

         pos= _instream->read(_outpos, buffer);
         // then rewind
         _outpos=0;
         _instream->goToSync(_startoffset);
         // and read leftover;
         return _instream->read( len-pos,buffer+pos);


}

//
//      We do nothing more that asking the input stream to go at the same place
//
//  if _byteoffset is > 0 we go to offset+byteoffset
// else byteoffset is <0 so it means we duplicate a  [0, byteoffset] seg
//
// if  _newoffset is [0,byteoffset] we go to it
// else we go to newoffset-byteoffset
//___________________________________________

uint8_t AVDMProcessAudio_RawShift::goToTime(uint32_t newoffset)
{
 return goTo(convTime2Offset(newoffset))   ;
 }

uint8_t AVDMProcessAudio_RawShift::goTo(uint32_t newoffset)
{

	if( _byteoffset>0)
 	{

 	 return _instream->goTo(_startoffset+newoffset+_byteoffset);
   }
	 else
  {
 			if(  newoffset< (uint32_t)(-_byteoffset))
    		{
        		// we need to buffer [0, byteoffset]
           	int32_t l;
        	
        		l=-_byteoffset-newoffset;
          		_outpos=l;
        		return _instream->goTo(_startoffset+newoffset);
          }
    			else
       {
         	_outpos=0; //  no need to duplicate
       	 return _instream->goTo(_startoffset+newoffset+_byteoffset);

         }

      }
};


AVDMProcessAudio_RawShift::~AVDMProcessAudio_RawShift()
{
    delete _wavheader;

}

//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_RawShift::read(uint32_t len, uint8_t * buffer)
{

    return readDecompress(len, buffer);

};
