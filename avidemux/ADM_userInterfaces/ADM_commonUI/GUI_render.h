/****************************************************************************
 copyright            : (C) 2001 by mean
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


#ifndef GUI_RENDER_H
#define GUI_RENDER_H

typedef enum renderZoom
{
        ZOOM_1_4,
        ZOOM_1_2,
        ZOOM_1_1,
        ZOOM_2,
        ZOOM_4,
        ZOOM_INVALID
};


uint8_t renderInit( void );
uint8_t renderResize(uint32_t w, uint32_t h,renderZoom newzoom);
uint8_t renderRefresh(void);
uint8_t renderExpose(void);
uint8_t renderUpdateImage(uint8_t *ptr);

uint8_t renderStartPlaying( void );
uint8_t renderStopPlaying( void );


typedef enum ADM_RENDER_TYPE
{
        RENDER_GTK=0,
#ifdef USE_XV
        RENDER_XV=1,
#endif
#ifdef USE_SDL
        RENDER_SDL=2,
#endif
        RENDER_LAST       


};

#endif