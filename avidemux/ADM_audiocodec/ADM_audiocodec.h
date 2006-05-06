/***************************************************************************
                          ADM_audiocodec.h  -  description
                             -------------------
    begin                : Fri May 31 2002
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
#ifndef ADMAUDIOCODEC
#define ADMAUDIOCODEC

#define  ADMAC_BUFFER (48000*4)
 class ADM_Audiocodec
 {
    protected:
					uint8_t 	_running;
     				uint8_t	_init;
                	
    public:
                				ADM_Audiocodec( uint32_t fourcc );
                	virtual			~ADM_Audiocodec() ;
         			virtual         void 	purge( void ) {};
                 virtual 			uint8_t beginDecompress( void )=0;
                 virtual 			uint8_t endDecompress( void )=0;
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL)=0;
                 virtual			uint8_t isCompressed( void )=0;
                 virtual			uint8_t isDecompressable( void )=0;
                

 };

 ADM_Audiocodec	*getAudioCodec(uint32_t fourcc,WAVHeader *info=NULL,uint32_t extra=0,uint8_t *extraData=NULL);

 class ADM_AudiocodecWav : public     ADM_Audiocodec
 {
    protected:
    public:
                						ADM_AudiocodecWav( uint32_t fourcc );;
                	virtual			~ADM_AudiocodecWav();
         			virtual         void 	purge( void );
                 virtual 			uint8_t beginDecompress( void );
                 virtual 			uint8_t endDecompress( void ) ;
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ) ;
                 virtual			uint8_t isDecompressable(void );

   };

class ADM_AudiocodecWavSwapped : public     ADM_Audiocodec
 {
    protected:
    						uint8_t _hold;
						uint8_t _instore;
    public:
                				ADM_AudiocodecWavSwapped( uint32_t fourcc );;
                virtual			~ADM_AudiocodecWavSwapped();
         	virtual         		void 	purge( void );
                 virtual 			uint8_t beginDecompress( void );
                 virtual 			uint8_t endDecompress( void ) ;
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ) ;
                 virtual			uint8_t isDecompressable(void );

   };

   class ADM_AudiocodecUnknown : public     ADM_Audiocodec
 {
    protected:				
    public:
                				ADM_AudiocodecUnknown( uint32_t fourcc )  : ADM_Audiocodec(fourcc){};;
                				~ADM_AudiocodecUnknown();
         			virtual         void 	purge( void ) {};
                 	 			uint8_t beginDecompress( void ) { return 0;};
                 	 			uint8_t endDecompress( void ){ return 0;};
                                                uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 				uint8_t isCompressed( void ){ return 1;};
                 				uint8_t isDecompressable(void ){ return 0;};

   };
#ifdef USE_AC3
#define ADM_AC3_BUFFER (50000*2)
    class ADM_AudiocodecAC3 : public     ADM_Audiocodec
 {
    protected:
        void        *ac3_handle;
        void        *ac3_sample;
        uint32_t    _downmix;

    public:
                                                ADM_AudiocodecAC3( uint32_t fourcc );
                 virtual                         ~ADM_AudiocodecAC3();
                 virtual         void            purge( void ) {};
                 virtual        uint8_t         beginDecompress( void );
                 virtual                        uint8_t endDecompress( void );
                 virtual    uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,
                                         ADM_ChannelMatrix *matrix=NULL);
                 virtual    uint8_t isCompressed( void ){ return 1;};
                 virtual    uint8_t isDecompressable(void ){ return 1;};

   };
#endif
#ifdef USE_LIBDCA
 class ADM_AudiocodecDCA : public     ADM_Audiocodec
 {
    protected:
        void        *dts_handle;

    public:
                                                ADM_AudiocodecDCA( uint32_t fourcc );
                 virtual                         ~ADM_AudiocodecDCA();
                 virtual         void            purge( void ) {};
                 virtual        uint8_t         beginDecompress( void );
                 virtual                        uint8_t endDecompress( void );
                 virtual    uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,
                                         ADM_ChannelMatrix *matrix=NULL);
                 virtual    uint8_t isCompressed( void ){ return 1;};
                 virtual    uint8_t isDecompressable(void ){ return 1;};

   };
#endif
    class ADM_Audiocodec8Bits : public     ADM_Audiocodec
 {
    protected:
    		uint8_t				_unsign;
    public:
                						ADM_Audiocodec8Bits( uint32_t fourcc );
                	virtual			~ADM_Audiocodec8Bits() ;
         			virtual         void 	purge( void ) {};
                 virtual 			uint8_t beginDecompress( void ) {return 1;};
                 virtual 			uint8_t endDecompress( void ) {return 1;}
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ){ return 1;};
                 virtual			uint8_t isDecompressable(void ){ return 1;};

   };

#ifdef USE_MP3

#define ADM_MP3_BUFFER (48*1024)
    class ADM_AudiocodecMP3 : public     ADM_Audiocodec
 {
    protected:
                                uint32_t        _head;
                                uint32_t        _tail;
                                uint8_t         _buffer[ADM_MP3_BUFFER];
                                void            *_stream;
                                void            *_frame;
                                void            *_synth;
    public:
                                                ADM_AudiocodecMP3( uint32_t fourcc );
                virtual                         ~ADM_AudiocodecMP3() ;
                virtual         void            purge( void ) {};
                 virtual        uint8_t         beginDecompress( void ) ;
                 virtual        uint8_t         endDecompress( void );
                 virtual        uint8_t         run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual        uint8_t         isCompressed( void ){ return 1;};
                 virtual        uint8_t         isDecompressable(void ){ return 1;};

   };
#endif

#ifdef USE_VORBIS
  class ADM_vorbis : public     ADM_Audiocodec
 {
    protected:
    				void 			*_contextVoid;         			
    public:
                					ADM_vorbis( uint32_t fourcc ,WAVHeader *info,
								uint32_t l,uint8_t *d);
                		virtual			~ADM_vorbis() ;
         			virtual         void 	purge( void ) {};
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, 
                         uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ){ return 1;};
                 virtual			uint8_t isDecompressable(void ){ return 1;};
		 virtual 			uint8_t beginDecompress( void ) { return 1;};
                 virtual 			uint8_t endDecompress( void );
          

   };



#endif
#ifdef USE_FAAD
#define FAAD_BUFFER 2048
  class ADM_faad : public     ADM_Audiocodec
 {
    protected:
    						uint8_t _inited;
						void	*_instance;
						uint8_t _buffer[FAAD_BUFFER];
						uint32_t _inbuffer;	
    public:
                					ADM_faad( uint32_t fourcc ,WAVHeader *info,
								uint32_t l,uint8_t *d);
                		virtual			~ADM_faad() ;
         			virtual         void 	purge( void );
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, 
                         uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ){ return 1;};
                 virtual			uint8_t isDecompressable(void ){ return 1;};
		 virtual 			uint8_t beginDecompress( void ) ;
                 virtual 			uint8_t endDecompress( void ) ;
          

   };



#endif

#define ADMWA_BUF (4*1024*16) // 64 kB internal
   class ADM_AudiocodecWMA : public     ADM_Audiocodec
 {
    protected:
                            void                    *_contextVoid;
                            uint8_t                 _buffer[ ADMWA_BUF];
                            uint32_t                _tail,_head;
                            uint32_t                _blockalign;
    public:
                                                ADM_AudiocodecWMA( uint32_t fourcc ,WAVHeader *info,uint32_t l,uint8_t *d);
                virtual                         ~ADM_AudiocodecWMA() ;
                virtual                         void 	purge( void ) {};
                virtual     uint8_t             beginDecompress( void ) ;
                virtual     uint8_t             endDecompress( void );
                virtual     uint8_t             run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                virtual     uint8_t             isCompressed( void ){ return 1;};
                virtual     uint8_t             isDecompressable(void ){ return 1;};

   };
   //****************************************************
   #define ADM_AMR_BUFFER (16*1024) // 16 kB internal
    class ADM_AudiocodecAMR : public     ADM_Audiocodec
 {
    protected:
                                void            *_contextVoid;
                                uint8_t         _buffer[ ADM_AMR_BUFFER];
                                uint32_t        _head,_tail;
    public:
                                                ADM_AudiocodecAMR( uint32_t fourcc ,WAVHeader *info);
                 virtual	               ~ADM_AudiocodecAMR() ;
                 virtual                        void 	purge( void ) {};
                 virtual                        uint8_t beginDecompress( void ) ;
                 virtual                        uint8_t endDecompress( void );
                 virtual                        uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual                        uint8_t isCompressed( void ){ return 1;};
                 virtual                        uint8_t isDecompressable(void ){ return 1;};

   };
   //****************************************************
  class ADM_AudiocodecUlaw : public     ADM_Audiocodec
 {
    protected:
    				uint32_t 		_inStock;                		
    public:
                				ADM_AudiocodecUlaw( uint32_t fourcc ,WAVHeader *info);
                	virtual			~ADM_AudiocodecUlaw() ;
         			virtual         void 	purge( void ) {};
                 virtual 			uint8_t beginDecompress( void ) {return 1;};
                 virtual 			uint8_t endDecompress( void ) {return 1;};
                 virtual 			uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr, 
                         uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                 virtual			uint8_t isCompressed( void ){ return 1;};
                 virtual			uint8_t isDecompressable(void ){ return 1;};

   };

 //****************************************************
#define IMA_BUFFER 4096*8
  class ADM_AudiocodecImaAdpcm : public     ADM_Audiocodec
 {
    protected:
                       uint32_t                _inStock,_me,_channels;
                       int                     ss_div,ss_mul; // ???
                                void            *_contextVoid;
                                uint8_t         _buffer[ IMA_BUFFER];
                                uint32_t        _head,_tail;

    public:
                                ADM_AudiocodecImaAdpcm( uint32_t fourcc ,WAVHeader *info);
                virtual         ~ADM_AudiocodecImaAdpcm() ;
                virtual void    purge( void ) {};
                virtual uint8_t beginDecompress( void ) {_head=_tail=0;return 1;};
                virtual uint8_t endDecompress( void ) {_head=_tail=0;return 1;};
                virtual uint8_t run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr, 
                                     uint32_t * nbOut,ADM_ChannelMatrix *matrix=NULL);
                virtual uint8_t isCompressed( void ){ return 1;};
                virtual uint8_t isDecompressable(void ){ return 1;};

   };

#endif
