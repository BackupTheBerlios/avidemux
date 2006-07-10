
/***************************************************************************
    copyright            : (C) 2002-6 by mean
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
#ifndef AUDMaudioAAC
#define AUDMaudioAAC

 //_____________________________________________
class AUDMEncoder_Faac : public AUDMEncoder
{
protected:
         uint32_t 	grab(uint8_t *obuffer);
         void           *_handle;
         
public:
                        uint8_t init(ADM_audioEncoderDescriptor *config);
                virtual ~AUDMEncoder_Faac();
                        AUDMEncoder_Faac(AUDMAudioFilter *instream);	
                virtual uint8_t	getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
                virtual uint8_t extraData(uint32_t *l,uint8_t **d);
};

#endif
