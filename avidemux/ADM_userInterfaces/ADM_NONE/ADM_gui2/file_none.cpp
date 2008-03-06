
// C++ Interface: 
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ADM_default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"

uint8_t FileSel_SelectDir(const char *title,char *target,uint32_t max, const char *source)
{ 
  return 0;
}
void GUI_FileSelRead(const char *label,SELFILE_CB cb) 
{}
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) 
{}
void GUI_FileSelRead(const char *label, char * * name)
{}
void GUI_FileSelWrite(const char *label, char * * name)
{}

uint8_t UI_getPhysicalScreenSize(uint32_t *w,uint32_t *h)
{
  *w=*h=10000; 
}