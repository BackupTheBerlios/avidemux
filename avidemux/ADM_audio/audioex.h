/***************************************************************************
                          audioex.h  -  description
                             -------------------
    begin                : Fri May 31 2002
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

 #include "ADM_audio/audiosource_ext.h"

class AVDMMP3AudioStream  : public AVDMFileStream
{
 public:
 			     AVDMMP3AudioStream( void ) :  AVDMFileStream() {};
         		~AVDMMP3AudioStream() { abort();}
           	virtual uint8_t open(char *name);
} ;

class AVDMWavAudioStream  : public AVDMFileStream
{
 public:
 			     AVDMWavAudioStream( void ) :  AVDMFileStream() {};
         		~AVDMWavAudioStream() { abort();}
           	virtual uint8_t open(char *name);
}  ;
#ifdef USE_AC3
class AVDMAC3AudioStream  : public AVDMFileStream
{
 public:
 			     AVDMAC3AudioStream( void ) :  AVDMFileStream() {};
         		~AVDMAC3AudioStream() { abort();}
           	virtual uint8_t open(char *name);
}  ;
#endif
