
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


#ifndef ASF_PACKET_H
#define ASF_PACKET_H
class asfPacket
{
  protected:
    uint32_t  readVCL(uint32_t bitwise);
    uint8_t   skip( uint32_t how);
    FILE        *_fd;
    uint32_t  packetStart;
    uint8_t   segmentId;
    uint32_t  pakSize;
  public:
    
    asfPacket(FILE *f,uint32_t pSize);
    ~asfPacket();
    uint8_t   dump(void);
    
    
  
    uint8_t   readChunkPayload(uint8_t *data, uint32_t *dataLen);
    uint8_t   nextPacket(void);
    uint8_t   skipPacket(void);
    
    uint32_t  getPos(void);
    uint32_t  getPayloadLen(void);
    
    uint64_t  read64(void);
    uint32_t  read32(void);
    uint32_t  read16(void);
    uint8_t   read8(void);
    uint8_t   read(uint8_t *where, uint32_t how);
};

#endif

