/***************************************************************************
                          toolame.hxx  -  description
                             -------------------
    begin                : Sun Jan 13 2002
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
#ifndef __ADM2LAME__
#define  __ADM2LAME__

class AVDMProcessAudio_Piper : public AVDMProcessAudioStream
{
protected:
				FILE 	*_pipe;
				 uint8_t  	_bufferin[PROCESS_BUFFER_SIZE];


public:
					uint8_t	push(uint32_t *eaten );
		virtual 		uint8_t 	init(char *outfile,uint32_t mode, uint32_t bitrate,char *extra=NULL);
							~AVDMProcessAudio_Piper();
        						AVDMProcessAudio_Piper(AVDMGenericAudioStream *instream	) ;

 		virtual uint32_t read(uint32_t len,uint8_t *buffer);
		virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer);

};


class AVDMProcessAudio_TooLame : public AVDMProcessAudio_Piper
{
protected:



public:

		virtual 		uint8_t 	init(char *outfile,uint32_t mode, uint32_t bitrate,char *extra=NULL);
        						AVDMProcessAudio_TooLame(AVDMGenericAudioStream *instream	)
								:AVDMProcessAudio_Piper(instream) {} ;


};

class AVDMProcessAudio_ExLame : public AVDMProcessAudio_Piper
{
protected:



public:

		 				uint8_t 	init(char *outfile,char *extra);
        						AVDMProcessAudio_ExLame(AVDMGenericAudioStream *instream	)
								:AVDMProcessAudio_Piper(instream) {} ;


};
class AVDMProcessAudio_External : public AVDMProcessAudio_Piper
{
protected:



public:

		 				uint8_t init(char *app,char *cmd);
        						AVDMProcessAudio_External(AVDMGenericAudioStream *instream	)
								:AVDMProcessAudio_Piper(instream) {} ;


};


#endif
