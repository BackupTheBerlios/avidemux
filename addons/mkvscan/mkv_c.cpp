/***************************************************************************
 *   Copyright (C) 2007 by fx   *
 *   fx@debian64   *
 *                                                                         *
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
#include "mkv_tags.h"

void ADM_mkvWalk(ADM_ebml_file *working, uint32_t size);

int main(int argc, char *argv[])
{
  ADM_ebml_file ebml;
  uint64_t id,len;
  ADM_MKV_TYPE type;
  const char *ss;
  
  if(!ebml.open(argv[1])) ADM_assert(0);
  
  // Read level 1 stuff
  while(!ebml.finished())
  {
      ebml.readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        printf("[MKV] Tag 0x%x not found\n",id);
        ebml.skip(len);
        continue;
      }
      printf("Found Tag : %x (%s)\n",id,ss);
      if(type==ADM_MKV_TYPE_CONTAINER)
      {
        uint64_t w=ebml.tell();
        ADM_mkvWalk(&ebml,len);
        ebml.seek(w+len);
      }else
        ebml.skip(len);
  }
  return 0;
}
/**

*/
#define recTab() for(int pretty=0;pretty<recurse;pretty++) printf("\t");
void ADM_mkvWalk(ADM_ebml_file *working, uint32_t size)
{
  uint64_t id,len;
  ADM_MKV_TYPE type;
  const char *ss;
  static int recurse=0;
  uint64_t pos; 
  
  recurse++;
   ADM_ebml_file son(working,size);
   while(!son.finished())
   {
      pos=son.tell();
      son.readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        recTab();printf("[MKV] Tag 0x%x not found\n",id);
        son.skip(len);
        continue;
      }
      recTab();printf("at 0x%llx, Found Tag : %x (%s) type %d (%s) size %d\n",pos,id,ss,type,ADM_mkvTypeAsString(type),len);
      uint32_t val;
      switch(type)
      {
        case ADM_MKV_TYPE_CONTAINER:
                  //if(id!=MKV_CLUSTER)
                    ADM_mkvWalk(&son,len);
                  //else
                  //    son.skip(len);
                  break;
        case ADM_MKV_TYPE_UINTEGER:
                  val=son.readUnsignedInt(len);
                  recTab();printf("\tval uint: %llu (0x%llx) \n",val,val);
                  break;
        case ADM_MKV_TYPE_INTEGER:
                  recTab();printf("\tval int: %lld \n",son.readSignedInt(len));
                  break;
        case ADM_MKV_TYPE_STRING:
        {
                  char string[len+1];
                  string[0]=0;
                  son.readString(string,len);
                  recTab();printf("\tval string: <%s> \n",string);
                  break;
        }
        default:
                son.skip(len);
                break;
      }
   }
   recurse--;
}

void bigHexPrint(uint64_t v)
{
  int s=56;
  int n=0,z;
  
  for(int i=0;i<8;i++)
  {
    z=(v>>s)&0xff;
    if(!z && !n) 
    {
      
    }else
    {
      printf("[%02x]", (v>>s)&0xff);
      n=1;
    }
    s-=8;
  }
  printf("\n"); 
}
//EOF
