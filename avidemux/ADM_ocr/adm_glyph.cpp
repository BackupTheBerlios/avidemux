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
 #include "ADM_assert.h"
 #include "ADM_library/default.h"
 #include "adm_glyph.h"
 //*************************************************
admGlyph::admGlyph(uint32_t w,uint32_t h)
{
    ADM_assert(w*h);
    width=w;
    height=h;
    data=new uint8_t[w*h];
    memset(data,0,w*h);
    code=NULL;
    next=NULL;
}
//*************************************************
admGlyph::~admGlyph()
{
    delete [] data;
    data=NULL;
    if(code)
    {
        ADM_dealloc(code);
        code=NULL;
    }
}
//*************************************************
uint8_t admGlyph::create(uint8_t *incoming, uint32_t stride)
{
uint8_t *in=incoming;
uint8_t *out=data;
    for(uint32_t y=0;y<height;y++)
    {
        memcpy(out,in,width);
        out+=width;
        in+=stride;
    }
    return 1;
}
//*************************************************
uint8_t  insertInGlyphTree(admGlyph *startGlyph, admGlyph *candidate)
{
admGlyph *old;
            old=startGlyph->next;
            candidate->next=old;
            startGlyph->next=candidate;
            return 1;
}
//*************************************************
uint8_t  destroyGlyphTree(admGlyph *startGlyph)
{
admGlyph *head,*tmp;
        if(!startGlyph) return 0;
        head=startGlyph->next;
        while(head)
        {
            tmp=head;
            head=head->next;
            delete tmp;
        }
    return 1;
}
//*************************************************
admGlyph *searchGlyph(admGlyph *startGlyph, admGlyph *candidate)
{
admGlyph *head=startGlyph->next;
            while(head)
            {   
                if(head->width==candidate->width && head->height==candidate->height)
                {
                    //Raw compare
                    if(!memcmp(head->data,candidate->data,head->width*head->height))
                        return head;
                
                }
                head=head->next;
            }
            return NULL;
}
/*************************************************/
admGlyph *clippedGlyph(admGlyph *in)
{
uint32_t w,h,lonecount,lone;
int32_t top,bottom,left,right;
admGlyph *nw=NULL;
            w=in->width;
            h=in->height;
            // Look if we got a lonely point at the first line
            lonecount=0;
            if(w>3)
              for(uint32_t i=0;i<w;i++) 
                if(in->data[i]) 
                {
                    lonecount++;
                    lone=i;
                }
            if(lonecount==1)
            {
                if(!lone) lone++;
                if(lone==w-1) lone--;
                if(!in->data[w+lone-1] && !in->data[w+lone] && !in->data[w+lone+1])
                    in->data[lone]=0;
            }
            // Go!
            left=0;
            while(columnEmpty(in->data+left,w,h) && left<w) left++;
            if(left==w) 
            {
                in->width=in->height=0;
                return in;
            }
            right=w-1;
            while(columnEmpty(in->data+right,w,h) && right>=left) right--;
            
            top=0;
            while(lineEmpty(in->data,w,w,top) && top<h) top++;
            
            bottom=h-1;
            while(lineEmpty(in->data,w,w,bottom) && bottom>=top) bottom--;
            
            nw=new admGlyph(right-left+1,bottom-top+1);
            nw->create(in->data+left+top*w,w);
            delete in;
            return nw;

}
/*************************************************/

