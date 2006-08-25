/***************************************************************************
              
    copyright            : (C) 2006 by mean
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
#include <ADM_assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/ADM_threads.h"
#include "ADM_packetQueue.h"
/*!
  Constructor for packetQueue
  \param name   : name of the packetQueue, useful for debugging
  \param nbSlot : How many packets can the packetQueue hold
  \param buffSize : BufferSize in bytes
 */

PacketQueue::PacketQueue(const char *name,uint32_t nbSlot,uint32_t buffSize)
{
  memset(this,0,sizeof(PacketQueue));
  _nbSlots=nbSlot;
  _bufferSize=buffSize;
  _name=ADM_strdup(name);
  _buffer=new uint8_t[_bufferSize];
  _slots=new Slots[_nbSlots];
  _mutex=new admMutex(_name);
  _pusherCond=new admCond(_mutex);
  _poperCond=new admCond(_mutex);
}
/*!
  Destructor
 */

PacketQueue::~PacketQueue()
{
  if(_mutex) delete _mutex;
  _mutex=NULL;
  if(_pusherCond) delete _pusherCond;
  _pusherCond=NULL;
  if(_poperCond) delete _poperCond;
  _poperCond=NULL;
  
  if(_name) ADM_dealloc(_name);
  _name=NULL;
  if(_buffer) delete [] _buffer;
  _buffer=NULL;
  if(_slots) delete [] _slots;
}
/*!
  Signal the pusher has finished, wake the poper.
  Warning : Must be called with mutex held
 */

uint8_t   PacketQueue::IsEmpty(void)
{
  uint8_t r=0;
  
  if(_slotHead!=_slotQueue)
  { 
      r=1;
  }
  
  return r;
}
/*!
  Signal the pusher has finished, wake the poper.

 */

uint8_t  PacketQueue::Finished(void)
{
  uint8_t r=0;
  _mutex->lock();
  _eof=1;
  _poperCond->abort();
  _mutex->unlock();
  return 1;
  
}
/*!
  Put a packet in the buffer
  Wait on _pusherCond if no slot available or buffer full
  \param ptr  : Ptr where to take the packet from
  \param size : packetsize

 */
uint8_t   PacketQueue::Push(uint8_t *ptr, uint32_t size)
{
  uint8_t r=0;
  uint32_t slot;
  uint32_t available;
  _mutex->lock();
  // First try to allocate a slot
  while(((_nbSlots+_slotHead-_slotQueue)%_nbSlots)==1)
  {
    _pusherCond->wait(); 
  }
  // Ok we have a slot,
  // Now lets's see if we have enough data in the buffer (we are still under lock)
  while(!_eof)
  {
      available=availableSpace();
      if(available>=size)
      {
         slot=_slotHead;
        _slotHead++;
        _slotHead%=_nbSlots;
        _slots[slot].size=size;
        _slots[slot].startAt=_bufferHead;
        if(_bufferSize>=(_bufferHead+size))
        {
          memcpy(&_buffer[ _bufferHead],ptr,size);
          _bufferHead+=size;
        }
        else  // Split
        {
          uint32_t part1=_bufferSize-_bufferHead;
          memcpy(&_buffer[ _bufferHead],ptr,part1);
          memcpy(&_buffer[ 0],ptr+part1,size-part1);
          _bufferQueue=size-part1;
          
        }
        // Look if someone was waiting ...
        if(_poperCond->iswaiting())
        {
          _poperCond->wakeup();
        }
        _mutex->unlock();
        return 1;
      }
      _pusherCond->wait();
  }
  _mutex->unlock();
  printf("[PKTQ] %s is eof\n",_name);
  return 0;
}
/*!
  Read a packet from the queue
  Wait on the _poperCond if empty
  If aborted returns immediatly
  \param ptr : Ptr where to put the packet
  \param size : returns packetsize

*/
uint8_t   PacketQueue::Pop(uint8_t *ptr, uint32_t *size)
{
  uint8_t r=0;
  uint32_t slot;
  uint32_t available,sz;
  _mutex->lock();
  // is there something ?
  while(IsEmpty() && !_eof)
  {
    _poperCond->wait();
  }
  if(_eof)
  {
    *size=0;
    _mutex->unlock();
    return 0;
  }
  // ok, which slot ?
  slot=_bufferQueue;
  sz=*size=_slots[slot].size;
  if(_bufferSize>=(_bufferQueue+sz))
  {
    memcpy(ptr,&(_buffer[_bufferQueue]),sz);
    _bufferQueue+=sz;
  }
  else  // Split
  {
    uint32_t part1=_bufferSize-_bufferQueue;
    memcpy(ptr,&_buffer[ _bufferQueue],part1);
    memcpy(ptr+part1,&_buffer[ 0],sz-part1);
    _bufferQueue=sz-part1;
  }
  // In case we made some space
  if(_pusherCond->iswaiting())
  {
    _pusherCond->wakeup();
  }
  _mutex->unlock();
  return 1;
}
/*!
  Returns available space in bytes in the buffer
  Warning, this method must be called with mutex
    hold!.

*/
uint32_t PacketQueue::availableSpace(void)
{
  
  uint32_t space=_bufferSize+_bufferHead-_bufferQueue;
  space=space%_bufferSize;
  space=_bufferSize-space;
  
  return space;
}
//EOF

