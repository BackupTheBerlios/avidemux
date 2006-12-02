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
static int audioCodec=0;
static int videoCodec=0;
//**************************************************
//**************************************************

//**************************************************
//**************************************************
void UI_setAProcessToggleStatus( uint8_t status )
{}
void UI_setVProcessToggleStatus( uint8_t status )
{}

//**************************************************




uint8_t UI_getPreviewToggleStatus( void )
{}
uint8_t UI_setPreviewToggleStatus( uint8_t status )
{}

uint8_t UI_getOutputToggleStatus( void )
{}
uint8_t UI_setOutputToggleStatus( uint8_t status )
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

// EOF
