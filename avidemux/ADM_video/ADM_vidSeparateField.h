/*

*/
  class  AVDMVideoSeparateField:public AVDMGenericVideoStream
 {

 protected:

        virtual char 					*printConf(void) ;

 public:
  						AVDMVideoSeparateField(  AVDMGenericVideoStream *in,CONFcouple *setup);
  						~AVDMVideoSeparateField();
		      virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									ADMImage *data,uint32_t *flags);

			virtual uint8_t 	configure( AVDMGenericVideoStream *instream) {return 0;};

 }     ;
   class  AVDMVideoMergeField:public AVDMGenericVideoStream
 {

 protected:

		ADMImage				*_uncompressed2;
		ADMImage				*_cache;
		uint32_t				_lastAsked;
        virtual char 					*printConf(void) ;

 public:
  						AVDMVideoMergeField(  AVDMGenericVideoStream *in,CONFcouple *setup);
  						~AVDMVideoMergeField();
		      virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									ADMImage *data,uint32_t *flags);

			virtual uint8_t configure( AVDMGenericVideoStream *instream) {return 0;};

 }     ;

