/***************************************************************************
                          op_savecopy.h  -  description
                             -------------------
    begin                : Fri May 3 2002
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
 #ifndef __AVI_SAVECPY__
 #define   __AVI_SAVECPY__

 class GenericAviSaveCopy : public   GenericAviSave
 {
     protected :
			
		          virtual uint8_t 	setupVideo( char *name  );
                          virtual uint8_t 	writeVideoChunk(uint32_t frame );
			  	  uint8_t	_needUserDataUpdate;

     public:
     						GenericAviSaveCopy()  :     GenericAviSave()
										{
											_needUserDataUpdate=0;
										};
						GenericAviSaveCopy(uint8_t pack)  :     GenericAviSave()
										{
											_needUserDataUpdate=pack;
										};	
               //           virtual ~GenericAviSaveCopy();
   };

 class GenericAviSaveCopyDualAudio : public   GenericAviSaveCopy
 {
     protected :

			
                        char				*_trackname;
			uint32_t			_audioCurrent2;


		                   virtual uint8_t setupAudio( void);
                        	 virtual uint8_t writeAudioChunk(uint32_t frame );
				 //virtual uint8_t setupVideo (char *name);
     public:
                                     GenericAviSaveCopyDualAudio(AVDMGenericAudioStream	*track);
                          //virtual ~GenericAviSaveCopyDualAudio();
   };



  #endif
