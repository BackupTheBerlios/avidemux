/***************************************************************************
                          ADM_edLoadSave.cpp  -  description
                             -------------------

	Save / load workbench

    begin                : Thu Feb 28 2002
    copyright            : (C) 2002 by mean
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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "fourcc.h"
#include "avio.hxx"
#include <ADM_assert.h>
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_inpics/ADM_pics.h"

#include "ADM_nuv/ADM_nuv.h"
#include "ADM_h263/ADM_h263.h"

#include "ADM_mpeg2dec/ADM_mpegscan.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_assert.h"
#include "prefs.h"

// Ugly but sooo usefull
extern uint32_t frameStart,frameEnd;
static uint32_t edFrameStart,edFrameEnd;
uint8_t loadVideoCodecConf( char *name);
uint8_t saveVideoCodecConf( char *name);

// Ugly, will have to clean it later

uint8_t ADM_Composer::getMarkers(uint32_t *start, uint32_t *end)
{
	if(_haveMarkers)
		{
			*start=edFrameStart;
			*end=edFrameEnd;		
		}
	else
		{
			*start=0;
			*end=_total_frames-1;
		}
	return 1;		
}
//______________________________________________
// Save the config, including name, segment etc...
//______________________________________________

uint8_t ADM_Composer::saveWorbench (char *name)
{
printf("\n **Saving workbench **\n");
  char *    tmp;

  if (!_nb_segment)
    return 1;

  FILE *    fd;

  fd = fopen (name, "wt");

  fprintf (fd, "ADMW0002\n");
  fprintf (fd,"%02ld videos\n", _nb_video);

  for (uint32_t i = 0; i < _nb_video; i++)
    {
      fprintf (fd, "Name : %s\n", _videos[i]._aviheader->getMyName ());
    }
fprintf (fd,"%02ld segments\n", _nb_segment);

for (uint32_t i = 0; i < _nb_segment; i++)
    {
      fprintf (fd, "Start : %lu\n", _segments[i]._start_frame);
      fprintf (fd, "Size : %lu\n", _segments[i]._nb_frames);
      fprintf (fd, "Ref :   %lu\n", _segments[i]._reference);
    }
// Dump video codec
	fprintf(fd,"Audio codec : %s\n",audioCodecGetName() );
// audio filter
	fprintf(fd,"Audio filter : %s\n",audioFilterGetName());
// audio conf
	fprintf(fd,"Audio conf : %s\n",audioCodecGetConf());


	fprintf(fd,"Video start-end : %lu %lu\n",frameStart,frameEnd);
	
// All done

  fclose (fd);
  // try to load filters
  tmp = (char *) ADM_alloc (strlen (name) + 10);
  ADM_assert (tmp);
  strcpy (tmp, name);
  strcat (tmp, ".flt");

  filterSaveXml (tmp, 1);
  
   // save codec settings if any
  strcpy (tmp, name);
  strcat (tmp, ".vcodec");

  saveVideoCodecConf(tmp);
  
  ADM_dealloc (tmp);
  return 1;


}


//______________________________________________
// Save the config, including name, segment etc...
//______________________________________________

uint8_t ADM_Composer::loadWorbench (char *name)
{

  char    str[4000];
  char    str_extra[4000];
  char    str_tmp[4000];
  char    filename2[1024];
  uint32_t    nb;
  FILE *    fd;
  char *    tmp;
  char *s;



  fd = fopen (name, "rt");
  if(!fd)
  {
	GUI_Alert("Trouble opening that file");
	return 0;

  }
  fgets(str,99,fd);
  if(strcmp(str,"ADMW0002\n"))
  {
	GUI_Alert("This does not look like\n a avidemux2 file...");
	fclose(fd);
	return 0;
  }
  // read nb_ videos
  fscanf (fd, "%02lu videos\n", &nb);
  printf ("WKB Reading workbench, %ld videos to go\n", nb);
  cleanup ();
  for (uint32_t i = 0; i < nb; i++)
    {
	    fgets (str_tmp, 390, fd);
	    // remove last char (CR or LF)
	    str_tmp[strlen(str_tmp)-1]=0;
	    s=str_tmp;
	    s+=strlen("Name : ");
	    printf("adding %s\n",s);
	    if(ADM_OK!=addFile (s))
	    {
		char *p;
		strncpy(filename2,name,sizeof(filename2));
		filename2[sizeof(filename2)-1] = '\0';
		if( (p = rindex(filename2,'/')) ){
			*(++p) = '\0';
		}else{
			filename2[0] = '\0';
		}
		if( (p = rindex(s,'/')) ){
			p++;
		}else{
			p = s;
		}
		strncat(filename2,p,sizeof(filename2)-strlen(filename2));
		filename2[sizeof(filename2)-1] = '\0';
		if( !strncmp(s,filename2,sizeof(filename2)) ){
			/* we don't have a second filename */
			GUI_Alert("Problem reading file,\nexpect crash");
			fclose(fd);
			_nb_video=i;
			if(_nb_video>1) _nb_video--;
			return 0;
		}
		printf(" could not open %s file..\n", s);
		printf(" adding %s instead\n", filename2);
		if(ADM_OK!=addFile (filename2)){
	    		GUI_Alert("Problem reading file,\nexpect crash");
			fclose(fd);
			_nb_video=i;
			if(_nb_video>1) _nb_video--;
			return 0;
		}
	   }

    }
    _nb_video=nb;

     // read nb_ seg
  fscanf (fd, "%02lu segments\n", &nb);
  printf ("WKB Reading workbench, %ld segments to go\n", nb);
   _nb_segment=nb;
 for (uint32_t i = 0; i < nb; i++)
    {
      fscanf (fd, "Start : %lu\n", &_segments[i]._start_frame);
      fscanf (fd, "Size : %lu\n", &_segments[i]._nb_frames);
      fscanf (fd, "Ref :   %lu\n", &_segments[i]._reference);
        updateAudioTrack (i);
	printf("segment :%lu done\n",i);
    }
// Dump video codec
	fscanf(fd,"Audio codec : %s\n",str );
 	audioCodecSetByName(str);
// read audio filter
	//fscanf(fd,"Audio filter : %s\n",str);
	fgets(str,200,fd);
 	 audioFilterSetByName( str);
// audio conf
	fgets(str,200,fd);
	audioCodecSetConf(str);


	if(2==fscanf(fd,"Video start-end : %lu %lu\n",&edFrameStart,&edFrameEnd))
	{
		uint8_t val = 0;
		prefs->get(FEATURE_IGNORESAVEDMARKERS,&val);
		if( val ){
			printf("Ignored start end!\n");
			_haveMarkers=0;
		}else{
			printf("Got start end!\n");
			_haveMarkers=1;
		}
	}
	else
	{
		_haveMarkers=0;
	}
//***
    printf(" .. all loaded\n");
  fclose (fd);
  _total_frames = 0;
  // update total frames
  for (uint32_t n = 0; n < _nb_segment; n++)
    {
      _total_frames += _segments[n]._nb_frames;
    }
  dumpSeg ();
  printf ("\n Total frames : %lu \n", _total_frames);

// try to load filters
  tmp = (char *) ADM_alloc (strlen (name) + 10);
  ADM_assert (tmp);
  strcpy (tmp, name);
  strcat (tmp, ".flt");


  filterLoadXml(tmp, 1);

 // load codec settings if any
  strcpy (tmp, name);
  strcat (tmp, ".vcodec");


  loadVideoCodecConf(tmp);
  
    
  ADM_dealloc (tmp);
  return 1;

}
