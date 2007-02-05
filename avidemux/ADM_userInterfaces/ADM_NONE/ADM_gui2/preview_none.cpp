#include "config.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <ADM_assert.h>
#include "fourcc.h"
#include "avio.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "GUI_render.h"

void UI_rgbDraw(void *widg,uint32_t w, uint32_t h,uint8_t *ptr) {}
void UI_updateDrawWindowSize(void *win,uint32_t w,uint32_t h) {}
void UI_getWindowInfo(void *draw, GUI_WindowInfo *xinfo) {}
void *UI_getDrawWidget(void ) {return NULL;}

void GUI_PreviewInit(uint32_t w , uint32_t h, uint32_t modal)
{}
uint8_t 	GUI_PreviewUpdate(uint8_t *data)
{
  return 1;
}
void 	GUI_PreviewEnd( void)
{}
uint8_t  	GUI_PreviewRun(uint8_t *data)
{
  return 1;
}
uint8_t GUI_PreviewStillAlive( void )
{
  return 1;
}
void GUI_PreviewShow(uint32_t w, uint32_t h, uint8_t *data)
{
}

