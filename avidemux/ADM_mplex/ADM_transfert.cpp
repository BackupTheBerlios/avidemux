//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
#include <ADM_assert.h>
#include "ADM_library/default.h"


#include"ADM_transfert.h"

//**************** Mutex *******************
admMutex::admMutex(void)
{
        ADM_assert(!pthread_mutex_init(&_tex,NULL));
        _locked=0;
}
admMutex::~admMutex()
{
        ADM_assert(!pthread_mutex_destroy(&_tex));
       
}

uint8_t admMutex::lock(void)
{

        ADM_assert(!pthread_mutex_lock(&_tex));
        _locked=1;
        return 1;
}
uint8_t admMutex::unlock(void)
{
        _locked=0;      // Just informative, race possible here
        ADM_assert(!pthread_mutex_unlock(&_tex));
        return 1;
}

//**************** Cond *******************

admCond::admCond( void )
{
        ADM_assert(!pthread_cond_init(&_cond,NULL));
        waiting=0;
        aborted=0;
}
admCond::~admCond()
{
        ADM_assert(!pthread_cond_destroy(&_cond));

}
uint8_t admCond::wait(void)
{
        if(aborted) return 0;
        // take sem
        _condtex.lock();
        waiting=1;
        ADM_assert(!pthread_cond_wait(&_cond, &(_condtex._tex)));
        waiting=0;
        _condtex.unlock();
        return 1;
}
uint8_t admCond::wakeup(void)
{
        ADM_assert(!pthread_cond_signal(&_cond));
        return 1;
}
uint8_t admCond::iswaiting( void)
{
        return waiting;
}
uint8_t admCond::abort( void )
{
        aborted=1;
        if(waiting) wakeup();
        return 1;

}

//**************** Transfert *******************
// *** Lot of race here : FIXME

Transfert::Transfert(void)
{
        buffer=new uint8_t[TRANSFERT_BUFFER];
        fill=0;
        aborted=0;
        waitingData=0;
        ready=1;
        fill=0;
}
Transfert::~Transfert()
{
        delete [] buffer;
        
}
uint32_t Transfert:: read(uint8_t *buf, uint32_t nb  )
{
uint32_t r=0;
        
  while(1)
  {
        mutex.lock();
        if(fill>=nb)
        {
                
                memcpy(buf,buffer,nb);
                memmove(buffer,buffer+nb,fill-nb);                
                fill-=nb;
                r+=nb;
                mutex.unlock();
                return r;
        }
        
        // Purge
         memcpy(buf,buffer,fill);
         buf+=fill;
         nb-=fill;
         r+=fill;
         
         fill=0;
         if(aborted) 
         {
                mutex.unlock();
                return r;
         }
         waitingData=1;       
         mutex.unlock();  
         cond.wait();
         if(aborted) 
         {         
                return r;
         }                 
         waitingData=0;
         
  }
  return 0;               
}
uint8_t Transfert::fillingUp( void)
{
        if(fill>(TRANSFERT_BUFFER*6)/10) return 1;
        return 0;

}
uint8_t Transfert:: write(uint8_t *buf, uint32_t nb  )
{
        if(aborted) return 0;
        
        mutex.lock(); 
        if(nb+fill>=TRANSFERT_BUFFER)
        {
                printf("\n When writting %lu bytes, we overflow the existing %lu bytes\n",nb,fill);
                ADM_assert(0);
        
        }
        memcpy(buffer+fill,buf,nb);        
        fill+=nb;
        mutex.unlock();
        if(waitingData)
                cond.wakeup();
        return 1;
}        
uint8_t Transfert::needData( void )
{
        return waitingData;
 
 }
uint8_t Transfert::abort( void )
{
        aborted=1;
        if(waitingData)
                cond.abort();
        return 1;
 
 }
  

