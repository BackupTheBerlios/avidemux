/***************************************************************************
                          op_savesmart.hxx  -  description
                             -------------------
    begin                : Mon May 6 2002
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
 #ifndef __AVI_SAVESMT__
 #define   __AVI_SAVESMT__

 class GenericAviSaveSmart : public   GenericAviSave
 {
     protected :
     					uint8_t	CPBuffer[MAXIMUM_SIZE*MAXIMUM_SIZE*3];
							uint8_t initEncoder(uint32_t qz );
							uint8_t stopEncoder(void  );
       				uint32_t encoderReady;
              uint32_t compEngaged;
	virtual 		uint8_t setupVideo( char *name  );
 	virtual 		uint8_t writeVideoChunk(uint32_t frame );
              encoder 	*_encoder;
              uint8_t _cqReenc          ;
     public:
     							
     					GenericAviSaveSmart(uint32_t qfactor);		
                          virtual ~GenericAviSaveSmart();
   };


  #endif
