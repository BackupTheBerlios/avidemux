/**
    \file ADM_psAudioProbe.cpp
    \brief Handle index file reading
    copyright            : (C) 2009 by mean
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

#include "ADM_default.h"
#include "ADM_audiodef.h"

#include "ADM_psAudioProbe.h"
/**
    \fn listOfPsAudioTracks
    \brief returns a list of audio track found, null if none found

*/
listOfPsAudioTracks *psProbeAudio(const char *fileName)
{
    listOfPsAudioTracks *tracks=new listOfPsAudioTracks;

    return tracks;
}


//EOF
