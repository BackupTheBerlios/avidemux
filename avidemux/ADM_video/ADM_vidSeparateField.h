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
          									uint8_t *data,uint32_t *flags);

			virtual uint8_t 	configure( AVDMGenericVideoStream *instream) {return 0;};

 }     ;
   class  AVDMVideoMergeField:public AVDMGenericVideoStream
 {

 protected:

		uint8_t					*_uncompressed2;
		uint8_t					*_cache;
		uint32_t					_lastAsked;
        virtual char 					*printConf(void) ;

 public:
  						AVDMVideoMergeField(  AVDMGenericVideoStream *in,CONFcouple *setup);
  						~AVDMVideoMergeField();
		      virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									uint8_t *data,uint32_t *flags);

			virtual uint8_t configure( AVDMGenericVideoStream *instream) {return 0;};

 }     ;

