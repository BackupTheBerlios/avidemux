/***************************************************************************
                          ADM_vidSRT.h  -  description
                             -------------------
    begin                : Thu Dec 12 2002
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
#ifdef USE_FREETYPE

#define SRT_MAX_LINE 3

 typedef struct subLine
 {
		uint32_t 		startTime;
		uint32_t 		endTime;
		char			*string;
	}subLine;
	
	typedef struct SUBCONF
	{
		uint32_t _fontsize;
		uint32_t _baseLine;
		int32_t    _Y_percent;
		int32_t    _U_percent;
		int32_t    _V_percent;

		char		 *_fontname;
		char		 *_subname;
		char 	*_charset;


	}SUBCONF;

  class  ADMVideoSubtitle:public AVDMGenericVideoStream
 {

 protected:

	    	SUBCONF								*_conf;

        virtual char 					*printConf(void) ;
       
        FILE									*_fd;
        uint8_t								loadSubTitle( void );
				uint8_t  							loadSRT( void )        ;
        uint32_t							_line;
        subLine								*_subs;
        uint32_t							_oldframe;
        uint32_t							_oldline;
        uint32_t							_bitmap;
        uint32_t							search(uint32_t time);
        void 									displayString(char *string);
        void 									displayChar(uint32_t w,uint32_t h,char c);
        void 									displayLine(char *string,uint32_t line, uint32_t len);
	uint8_t 							lowPass(uint8_t *src, uint8_t *dst, uint32_t w, uint32_t h);
	uint8_t 							decimate(uint8_t *src, uint8_t *dst, uint32_t w, uint32_t h);


        uint8_t								*_bitmapBuffer;
	uint8_t								*_maskBuffer;
	uint8_t								blend(uint8_t *target,uint32_t baseLine);
	ADMfont								*_font;
	uint8_t 								loadSubtitle(void);
	uint8_t								loadFont(void);

	uint8_t 								subParse( subLine *in, char *string );

 public:
 		
  						ADMVideoSubtitle(  AVDMGenericVideoStream *in,CONFcouple *setup);
  						~ADMVideoSubtitle();
		      virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          											uint8_t *data,uint32_t *flags);

			virtual uint8_t	getCoupledConf( CONFcouple **couples)		;
			virtual uint8_t 	configure( AVDMGenericVideoStream *instream);
							
 }     ;
#endif
