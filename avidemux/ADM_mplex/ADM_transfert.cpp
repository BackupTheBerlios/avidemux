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
#define HIGH_LVL        ((TRANSFERT_BUFFER*2)/3)
#define LOW_LVL         (TRANSFERT_BUFFER/3)
// minimum amount of audio buffer we need
#define MIN_REQUIRED    (32*1024)
//#define MPLEX_D

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

        aborted=0;
        
        
        head=tail=0;
        
}
Transfert::~Transfert()
{
        delete [] buffer;
        
}
uint32_t Transfert:: read(uint8_t *buf, uint32_t nb  )
{
uint32_t r=0;
uint32_t fill=0;
#ifdef MPLEX_D                 
         printf("Reading %lu\n",nb);
#endif          
        
  while(1)
  {
        mutex.lock();
        fill=tail-head;        
        if(fill>=nb)
        {
                
                memcpy(buf,buffer+head,nb);                
                head+=nb;
                r+=nb;
                
                goto endit;                                                    
        }
        
        // Purge
         memcpy(buf,buffer+head,fill);
         buf+=fill;
         nb-=fill;
         r+=fill;         
         head=tail=0;
         if(aborted) 
         {
                
                goto endit;
         }
         
         ADM_assert(!clientCond.iswaiting());
         mutex.unlock();  
#ifdef MPLEX_D      
         printf("Wanted : %lu , left :%lu\n",nb,fill);   
         printf("Slave sleeping\n");
#endif         
         
         cond.wait();         
         if(aborted) 
         {         
                mutex.lock();
                goto endit;
         }                          
         
  }
endit: 
        if(clientCond.iswaiting()) // No need to protect as the client is locked
        {
                fill=tail-head;       
                if(fill<LOW_LVL)
                {
                        printf("Waking..\n");
                        clientCond.wakeup();        
                }
        }
        mutex.unlock();
        return r;               
}
//*********************************
uint8_t Transfert::fillingUp( void)
{
uint8_t r=0;

        mutex.lock();
        if((tail-head)>HIGH_LVL)
                r=1;
        else r=0;
        mutex.unlock();
        return r;
        

}
uint8_t Transfert:: write(uint8_t *buf, uint32_t nb  )
{
        if(aborted) return 0;
        
#ifdef MPLEX_D                 
         printf("Writing %lu\n",nb);
#endif          
        mutex.lock(); 
        // Need to pack ?
        if(nb+tail>=TRANSFERT_BUFFER)
        {
                memmove(buffer,buffer+head,tail-head);
                tail-=head;
                head=0;
        }
        // Overflow ?
        if(nb+tail>=TRANSFERT_BUFFER)
        {
                printf("\n When writting %lu bytes, we overflow the existing %lu bytes\n",nb,tail-head);
                ADM_assert(0);
        
        }
        memcpy(buffer+tail,buf,nb);        
        tail+=nb;
        
        if(cond.iswaiting())
        {
#ifdef MPLEX_D      
           
         printf("Slave waking\n");
#endif     
                cond.wakeup();
        }                
        mutex.unlock();
        return 1;
}        
uint8_t Transfert::needData( void )
{
  int32_t l;
  uint8_t r=0;
        mutex.lock(); 
        l=tail-head;
        ADM_assert(l>=0);
        mutex.unlock();
        if(l<MIN_REQUIRED) r=1;
        if(cond.iswaiting()) r=1;
//        return cond.iswaiting();
        return r;
 
 }
uint8_t Transfert::abort( void )
{
        aborted=1;
        if(cond.iswaiting())
                cond.abort();
        return 1;
 
 }
  
uint8_t Transfert::clientLock( void )
{
#ifdef MPLEX_D               
         printf("Slave sleeping (%lu)\n",tail-=head);
#endif         

        clientCond.wait();               
        return 1;

}
