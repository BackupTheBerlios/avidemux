/***************************************************************************
                          audioeng_process.h
                             -------------------
    begin                : 2006
    copyright            : (C) 2002-2006 by mean
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

#ifndef __Audio_ENG_Process__
#define __Audio_ENG_Process__

#define AUD_PROCESS_BUFFER_SIZE 48000*2*4 // should be enougth 4 seconds of stereo

typedef enum AUD_Status
{
    AUD_OK=1,
    AUD_ERROR,
    AUD_NEED_DATA,  // Useless?
    AUD_END_OF_STREAM
};
/*
    incoming --> incomingBuffer  || Processing  --> fill (into next in chain buffer)
          
                   
*/



class AUDMAudioFilter
{
  protected:
    float           _incomingBuffer[AUD_PROCESS_BUFFER_SIZE];
    uint32_t        _head,_tail;    
    WAVHeader       _wavHeader;     // _wavHeader->byterate holds the size in # of float (NOT SAMPLE)
    
    AUDMAudioFilter *_previous;
  public:
                                AUDMAudioFilter(AUDMAudioFilter *previous);
         virtual                ~AUDMAudioFilter();
                    uint8_t    shrink( void);
         virtual    uint32_t   fill(uint32_t max,float *output,AUD_Status *status)=0;      // Fill buffer: incoming -> us
                                                                                           // Output MAXIMUM max float value
                                                                                           // Not sample! float!
         
         virtual    WAVHeader  *getInfo(void) {return &_wavHeader;}
         virtual    uint8_t    rewind(void)  ;                                              // go back to the beginning
};
#endif
