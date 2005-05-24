/***************************************************************************
                         Base demuxer class
    
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include <ADM_assert.h>


#include "dmx_demuxer.h"

dmx_demuxer::dmx_demuxer(void)
{
        _size=0;
        _lastErr=0;
}
dmx_demuxer::~dmx_demuxer(void)
{
}
uint8_t         dmx_demuxer::sync( uint8_t *stream)
{
uint32_t val,hnt;


                val=0;
                hnt=0;                  
                        
                // preload
                hnt=(read8i()<<16) + (read8i()<<8) +read8i();
                if(_lastErr)
                {
                        _lastErr=0;
                        printf("\n io error , aborting sync\n");
                        return 0;       
                }
                
                while((hnt!=0x00001))
                {
                                        
                        hnt<<=8;
                        val=read8i();                                   
                        hnt+=val;
                        hnt&=0xffffff;  
                                        
                        if(_lastErr)
                        {
                             _lastErr=0;
                            printf("\n io error , aborting sync\n");
                            return 0;
                         }
                                                                        
                }
                                
                *stream=read8i();
                return 1;
}
