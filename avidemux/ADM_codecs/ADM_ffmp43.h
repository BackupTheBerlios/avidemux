/***************************************************************************
                          ADM_ffmp43.h  -  description
                             -------------------
                             
	Mpeg4 ****decoder******** using ffmpeg
	                              
    begin                : Wed Sep 25 2002
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
#ifdef USE_FFMPEG


 class decoderFF : public decoders
{
     protected:
                                        uint8_t                 _refCopy;
					AVCodecContext  	*_context;
					AVFrame			_frame;
					uint8_t 		*_internalBuffer;					
					uint8_t			_allowNull; 	
					uint32_t		_swapUV;	
					uint32_t  		frameType( void );
					uint8_t                 clonePic(AVFrame *src,ADMImage *out);
					uint32_t		_showMv;
                                        uint32_t                _gmc;
     public:
                                        
     					decoderFF(uint32_t w,uint32_t h);
         		virtual		~decoderFF();
                        virtual         uint8_t dontcopy(void) { return _refCopy;}   
    			virtual 	uint8_t 	uncompress(uint8_t *in,ADMImage *out,
						uint32_t len,uint32_t *flag=NULL) 		;
   		       virtual 		void 	setParam( void );
		       virtual 		uint8_t bFramePossible(void) { return 0;}
			virtual 	uint8_t decodeHeaderOnly(void ) ;
			virtual 	uint8_t decodeFull(void );
			virtual 	uint8_t isDivxPacked( void );
			virtual		uint32_t getSpecificMpeg4Info( void );
}   ;

class decoderFFDiv3 : public  decoderFF
{
     protected:
				
			
     public:
     						   
                      				decoderFFDiv3(uint32_t w,uint32_t h)  ;
               				
}   ;
class decoderFFMpeg4VopPacked : public  decoderFF
{
     protected:


     public:
     							decoderFFMpeg4VopPacked(uint32_t w,uint32_t h) ;
				// mpeg4 can have B-frame
		 virtual 		uint8_t 	bFramePossible(void) { return 0;}
				 // Vop packed are not indexable
		virtual 			uint8_t isIndexable( void ){ return 0;};
}   ;
class decoderFFMpeg4 : public  decoderFF
{
     protected:


     public:
     		          				decoderFFMpeg4(uint32_t w,uint32_t h) ;
			// mpeg4 can have B-frame
			 virtual 	uint8_t 	bFramePossible(void) { return 1;}

}   ;
class decoderFFMpeg12 : public  decoderFF
{
     protected:


     public:
			  			decoderFFMpeg12(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;
				// mpeg1/2 can have B-frame
		 virtual 	uint8_t 	bFramePossible(void) { return 1;}

}   ;
class decoderFFSVQ3 : public  decoderFF
{
     protected:


     public:
     				          	decoderFFSVQ3(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;
		 virtual 	uint8_t 	bFramePossible(void) { return 0;}

}   ;

class decoderFFDV : public decoderFF
{
	   protected:
				
			
     public:
     				          	 decoderFFDV(uint32_t w,uint32_t h,uint32_t l,uint8_t *d);
	
}     ;
class decoderFFMP42 : public decoderFF
{
	   protected:
				
			
     public:
     				          	decoderFFMP42(uint32_t w,uint32_t h) ;
	
}   ;
class decoderFFH263 : public decoderFF
{
	   protected:
				
			
     public:
     				          	decoderFFH263(uint32_t w,uint32_t h) ;
	
}   ;
class decoderFFH264 : public decoderFF
{
  protected:
                                
                        
  public:
    decoderFFH264(uint32_t w,uint32_t h) ;
        
}   ;
class decoderFFhuff : public decoderFF
{
	   protected:
				
			
     public:
     				         	decoderFFhuff(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;

}   ;
class decoderFF_ffhuff : public decoderFF
{
           protected:
                                
                        
     public:
                                                decoderFF_ffhuff(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;

}   ;
class decoderFFWMV2 : public decoderFF
{
	   protected:


     public:
     				          	decoderFFWMV2(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;

}   ;
class decoderFFV1 : public decoderFF
{
	   protected:


     public:
     				          	decoderFFV1(uint32_t w,uint32_t h) ;

}   ;
class decoderFFMJPEG : public decoderFF
{
	   protected:


     public:
     				          	decoderFFMJPEG(uint32_t w,uint32_t h) ;

}   ;
class decoderSnow : public decoderFF
{
	   protected:


     public:
     				          	decoderSnow(uint32_t w,uint32_t h) ;

}   ;
class decoderFFcyuv : public decoderFF
{
	   protected:
				
			
     public:
     				          	decoderFFcyuv(uint32_t w,uint32_t h,uint32_t l,uint8_t *d) ;
	
}   ;

#endif
