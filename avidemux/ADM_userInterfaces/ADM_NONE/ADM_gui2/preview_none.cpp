#include "config.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"

#include <ADM_assert.h>
#include "GUI_render.h"

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

uint8_t renderInit( void )
{
  return 1;
}
uint8_t renderResize(uint32_t w, uint32_t h,renderZoom newzoom)
{
  return 1;
}
uint8_t renderRefresh(void)
{
  return 1;
}
uint8_t renderExpose(void)
{
  return 1;
}
uint8_t renderUpdateImage(uint8_t *ptr)
{
  return 1;
}
uint8_t renderUpdateImageBlit(uint8_t *ptr,uint32_t startx, uint32_t starty, uint32_t w, uint32_t h,uint32_t primary)
{
  return 1; 
}
uint8_t renderStartPlaying( void )
{
  return 1;
}
uint8_t renderStopPlaying( void )
{
  return 1;
}

