//
// C++ Interface: audioeng_film2pal
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class AVDMProcessAudio_Film2Pal : public AVDMBufferedAudioStream
{
protected:
			       
				 double  _current;				
				 double  _target;
				 uint8_t  _bufferin[PROCESS_BUFFER_SIZE];
        	    		 uint32_t 	grab(uint8_t *obuffer);
		
public:
		
			~AVDMProcessAudio_Film2Pal();				
		        AVDMProcessAudio_Film2Pal(AVDMGenericAudioStream *instream	);		
     		
};
