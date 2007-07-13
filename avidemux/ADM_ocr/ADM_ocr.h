//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ADM_OCR_H
#define ADM_OCR_H
#include "adm_glyph.h"


typedef enum 
{
        ReplyOk=1,
        ReplyClose=0,
        ReplyCalibrate=2,
        ReplySkip=3,
        ReplySkipAll=4
}ReplyType;

ReplyType handleGlyph(uint8_t *workArea,uint32_t start, uint32_t end,uint32_t w,uint32_t h,uint32_t base);
ReplyType ocrBitmap(uint8_t *data,uint32_t w,uint32_t h,char *decodedString);
uint8_t   mergeBitmap(uint8_t *bitin, uint8_t *bitout, uint8_t *maskin,uint32_t w, uint32_t h);
void ocrUpdateMinThreshold(void);
#endif 
