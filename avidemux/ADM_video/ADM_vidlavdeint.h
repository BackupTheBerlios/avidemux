//
//
// C++ Interface: ADM_vidlavdeint
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

class  AVDMVideoLavDeint:public AVDMGenericVideoStream
 {

 protected:


        virtual char 		*printConf(void) ;

 public:
  				AVDMVideoLavDeint(  AVDMGenericVideoStream *in,CONFcouple *setup);
  				~AVDMVideoLavDeint();
	virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									uint8_t *data,uint32_t *flags);

			virtual uint8_t 	configure( AVDMGenericVideoStream *instream) 
								{return 0;};


 }     ;

 
