// C++ Interface: 
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "GUI_ui.h"

static ADM_OUT_FORMAT format=ADM_AVI;
//**************************************************
int 	UI_getCurrentACodec(void)
{
  return 0; 
}
//**************************************************
uint8_t 	UI_SetCurrentFormat( ADM_OUT_FORMAT fmt )
{
format=fmt;
}

ADM_OUT_FORMAT 	UI_GetCurrentFormat( void )
{
  return format;
}
//**************************************************
int 	UI_getCurrentVCodec(void)
{
  return 0; 
}


void UI_updateFrameCount(uint32_t curFrame)
{}
void UI_setFrameCount(uint32_t curFrame,uint32_t total)
{}

void UI_updateTimeCount(uint32_t curFrame, uint32_t fps)
{}
void UI_setTimeCount(uint32_t curFrame,uint32_t total, uint32_t fps)
{}

double 	UI_readScale( void )
{
  return 0;
}
void 	UI_setScale( double  val )
{}
void 	UI_setFrameType( uint32_t frametype,uint32_t qp)
{}
void 	UI_setMarkers(uint32_t a, uint32_t b )
{}
void 	UI_setTitle(char *name)
{}


uint8_t UI_getPreviewToggleStatus( void )
{}
uint8_t UI_setPreviewToggleStatus( uint8_t status )
{}

uint8_t UI_getOutputToggleStatus( void )
{}
uint8_t UI_setOutputToggleStatus( uint8_t status )
{}


void UI_setAProcessToggleStatus( uint8_t status )
{}
void UI_setVProcessToggleStatus( uint8_t status )
{}

void UI_iconify( void )
{}
void UI_deiconify( void )
{}

uint32_t UI_readCurFrame( void )
{
    return 0;
}
void UI_JumpDone(void)
{}


void UI_toogleSide(void)
{}
void UI_toogleMain(void)
{}

uint8_t UI_getTimeShift(int *onoff,int *value)
{
  *onoff=0;
  *value=0;
  return 1;
}
uint8_t UI_setTimeShift(int onoff,int value)
{
  return 1;
}

uint8_t UI_updateRecentMenu( void )
{
  return 1;
}

uint8_t UI_shrink(uint32_t w,uint32_t h)
{
  return 1;
}
uint8_t UI_arrow_enabled(void)
{
  return 1;
}
uint8_t UI_arrow_disabled(void)
{
  return 1;
}
void UI_purge( void )
{}
void UI_setAudioCodec( int i)
{}
void UI_setVideoCodec( int i)
{}

// EOF
