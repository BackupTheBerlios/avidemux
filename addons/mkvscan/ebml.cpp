/***************************************************************************
 *   Copyright (C) 2007 by mean,    *
 *   fixounet@free.fr   *
 *                                                                         *
 *        EBML Handling code                                               *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#define ADM_assert assert
#include "ADM_ebml.h"
#define aprintf(...) {}

/*
  It is slow , optimize later
*/  

uint8_t    ADM_ebml::readElemId(uint64_t *code,uint64_t *len)
{
  *code=readEBMCode_Full(); // Keep the length-bits, easier to match with documentation
  *len=readEBMCode();
  return 1;
}

/**
      \fn       readEBMCode
      \brief    Returns unsigned integer
*/

uint64_t    ADM_ebml::readEBMCode(void)
{
 uint64_t start=readu8();
  uint64_t val;      
  uint32_t mask=0x80,outmask=0x7F,more=0;
  
  aprintf("Start :%x at %llx\n",start,tell()-1);
  
  while(!(mask&start))
  {
    mask>>=1;
    ADM_assert(mask);
    more++; 
  }
  outmask=mask-1;
  start=start&outmask;
  for(int i=0;i<more;i++)
  {
    start=(start<<8)+readu8(); 
  }
  aprintf("End at %llx\n",tell());
  return start;
}
/**
      \fn       readEBMCode
      \brief    Returns unsigned integer
*/

int64_t    ADM_ebml::readEBMCode_Signed(void)
{
  uint8_t start=readu8();
  int64_t val;      
  uint32_t mask=0x80,outmask=0x7F,more=0;
  
  while(!(mask&start))
  {
    mask>>=1;
    ADM_assert(mask);
    more++; 
  }
  outmask=mask-1;
  val=start&outmask;
  for(int i=0;i<more;i++)
  {
    val=(val<<8)+readu8(); 
  }
  // Signed !
  //FIXME
  return val;
}
/**
      \fn       readEBMCode_Full
      \brief    Returns complete code, including size-bits (used only for EBML_ID)
*/
uint64_t    ADM_ebml::readEBMCode_Full(void)
{
  uint64_t start=readu8();
  uint32_t mask=0x80,more=0;
  aprintf(">>StartFull :%x at %llx\n",start,tell()-1);
  while(!(mask&start))
  {
    mask>>=1;
    ADM_assert(mask);
    more++; 
  }
  for(int i=0;i<more;i++)
  {
    start=(start<<8)+readu8(); 
  }
  return start;
}

uint64_t    ADM_ebml::readUnsignedInt(uint32_t nb) 
{
  uint64_t val=0;
  for(int i=0;i<nb;i++)
  {
    val=(val<<8)+readu8(); 
  }
  return val;
}
int64_t    ADM_ebml::readSignedInt(uint32_t nb) 
{
  return 0;
}
uint8_t     ADM_ebml::readString(char *string, uint32_t maxLen)
{
  uint8_t v;
  while(maxLen--)
  {
    v=*string++=readu8();
    if(!v) return 1;
  }
  *string=0;
  return 1;  
}
/******************
  Low level read
**********************/


uint8_t ADM_ebml::readu8(void)
{
  uint8_t v;
    readBin(&v,1);
    return v;
}
uint16_t ADM_ebml::readu16(void)
{
  uint8_t v[2];
    readBin(v,2);
    return (uint16_t)(v[0]<<8)+v[1];
}
ADM_ebml::ADM_ebml(void)
{
  
}
ADM_ebml::~ADM_ebml()
{
  
}


//*******************************************
//***********FILE IO PART *******************
//*******************************************
ADM_ebml_file::ADM_ebml_file(ADM_ebml_file *father,uint32_t size)
{
  _close=0;
  _size=size;
  fp=father->fp;
   _begin=ftello(fp);
}
ADM_ebml_file::ADM_ebml_file(void) : ADM_ebml()
{
  
}
ADM_ebml_file::~ADM_ebml_file()
{
  ADM_assert(fp);
  if(_close) fclose(fp);
  else fseeko(fp,_begin+_size,SEEK_SET);
  fp=NULL; 
}
uint8_t ADM_ebml_file::open(const char *name)
{
  _close=1;
  fp=fopen(name,"rb");
  if(!fp) 
  {
    aprintf("[EBML FILE] Failed to open <%s>\n",name);
    return 0;
  }
  fseeko(fp,0,SEEK_END);
  _begin=0;
  _size=ftello(fp);
  fseeko(fp,0,SEEK_SET);
  return 1;
}
uint8_t  ADM_ebml_file::readBin(uint8_t *whereto,uint32_t len)
{
  ADM_assert(fp);
  if(!fread(whereto,len,1,fp)) return 0;
  return 1;
}

uint8_t ADM_ebml_file::skip(uint32_t vv)
{
  fseeko(fp,vv,SEEK_CUR);
  return 1; 
}
uint64_t ADM_ebml_file::tell(void)
{
  return ftello(fp);
}
uint8_t ADM_ebml_file::seek(uint64_t pos)
{
  fseeko(fp,pos,SEEK_SET);
  return 1;
}
uint8_t ADM_ebml_file::finished(void)
{
  if(tell()>(_begin+_size-4)) return 1;
  return 0; 
}
//EOF
