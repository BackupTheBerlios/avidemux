/***************************************************************************
                          op_mpeg_write.h  -  description
                             -------------------
    begin                : Sat Aug 3 2002
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


typedef enum
{
	ADM_VCD=1,
	ADM_SVCD,
	ADM_DVD

}ADM_MPEGTYPE;

class	mpegWritter
{
  private :
  				
				Mpeg2enc		*_codec;
				uint32_t 		_w,_h,_page;
				uint32_t		_fps1000;
				uint32_t		_total;
				uint8_t			_outputAsPs;
				uint8_t			*_buffer;
				uint8_t			*_buffer_out;
				uint8_t			init(char *name,ADM_MPEGTYPE type,
				uint8_t 		interlaced,
				uint8_t 		bff, // WLA
				uint8_t 		widescreen);
				uint8_t			end( void );
				uint8_t			save_regular(char *name, ADM_MPEGTYPE mpegtype,
							int qz, int bitrate,int matrix,uint8_t interlaced,
								uint8_t bff, // WLA
								uint8_t widescreen);
				uint8_t  		save_dualpass(char *name,uint32_t final_size,
								uint32_t maxbitrate,
								ADM_MPEGTYPE mpegtype,
								int matrix,uint8_t interlaced,
								uint8_t bff, // WLA
								uint8_t widescreen);
				uint8_t  		save(char *name,ADM_MPEGTYPE mode);
				uint8_t  		dopass1(char *name,char *statname,
								uint32_t final_size,uint32_t maxbitrate,
								ADM_MPEGTYPE mpegtype,
								int matrix,uint8_t interlaced,
								uint8_t bff, // WLA
								uint8_t widescreen);
				uint8_t  		dopass2(char *name,char *statname, 
								uint32_t final_size,uint32_t maxbitrate,
								ADM_MPEGTYPE mpegtype,
								int matrix,uint8_t interlaced,
								uint8_t bff, // WLA
								uint8_t widescreen);
				uint8_t 		initLveMux( char *name );								
				AVDMGenericAudioStream	*_audio;
				FILE			*_lvepipe;
				FILE			*_audioFifo;
				FILE			*_videoFifo;
				uint32_t		_audioOneFrame;
				uint8_t			*_audioBuffer;
							  									
	public:
							mpegWritter( void );
							mpegWritter( uint8_t ps_stream );	
							~mpegWritter();
			uint8_t 			save_vcd(char *name);
			uint8_t 			save_svcd(char *name);
			uint8_t 			save_dvd(char *name);
};

