#ifndef VID_PDW_
#define VID_PDW_

 class  ADMVideoPullDown:public AVDMGenericVideoStream
 {

 protected:

		uint8_t					*_uncompressed[5];
     		virtual 	char 			*printConf(void);
		uint32_t					_cacheStart;

 public:


  				ADMVideoPullDown(  AVDMGenericVideoStream *in,CONFcouple *setup);

  	virtual 		~ADMVideoPullDown();
	virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          								uint8_t *data,uint32_t *flags);
	virtual uint8_t configure( AVDMGenericVideoStream *instream) {return 1;};

 }     ;
#endif
