//
// C++ Interface: op_ogsave
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OP_OGSAVE
#define OP_OGSAVE

#include "ADM_dialog/DIA_encoding.h"
#include "oplug_ogm/op_ogpage.h"
#include "ADM_encoder/adm_encoder.h"
class ADM_ogmWrite
{
	protected:
			
			uint32_t		_togo;
			FILE			*_fd;
			uint32_t		_fps1000;
			uint8_t 		*_audioBuffer;
			uint8_t 		*_videoBuffer;
			DIA_encoding		*encoding_gui;
			ogm_page 		*videoStream;
			ogm_page		*audioStream;
			
			//virtual uint8_t	initAudio(void)=0;
			virtual uint8_t		initVideo(void);
			//virtual uint8_t	writeAudio(void)=0;
			virtual uint8_t	writeVideo(uint32_t frame);	
	public:
					ADM_ogmWrite(void);				
					~ADM_ogmWrite(void);
			uint8_t 	save(char *name);
};

class ADM_ogmWriteCopy : public ADM_ogmWrite
{
protected:
			
			virtual uint8_t	initVideo(void);			
			virtual uint8_t	writeVideo(uint32_t frame);	
public:
			ADM_ogmWriteCopy(void);
			~ADM_ogmWriteCopy(void);
};

class ADM_ogmWriteProcess : public ADM_ogmWrite
{
protected:
			AVDMGenericVideoStream  *_incoming;
			Encoder 		*_encode;
			virtual uint8_t	initVideo(void);			
			virtual uint8_t	writeVideo(uint32_t frame);	
public:
			ADM_ogmWriteProcess(void);
			~ADM_ogmWriteProcess(void);


};


#endif
