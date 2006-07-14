/***************************************************************************
                          gui_navigate.cpp  -  description
                             -------------------

            GUI Part of get next frame, previous key frame, any frame etc...

    begin                : Fri Apr 12 2002
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
#include <math.h>


#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"

#include "ADM_assert.h"

#include "prototype.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
#include "gui_action.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_dialog/DIA_working.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
//____________________________________

uint8_t GUI_getFrame(uint32_t frameno, ADMImage *image, uint32_t *flags)
{
uint32_t len;

	//return video_body->getUncompressedFrame(frameno,image,flags);
	AVDMGenericVideoStream *filter=getFirstCurrentVideoFilter( );
	ADM_assert(filter);
	return filter->getFrameNumberNoAlloc(frameno,&len,image,flags);
	

}
void GUI_NextFrame(void)
{
//    uint8_t *ptr;
uint32_t flags;
    if (playing)
	return;

    if( avifileinfo && curframe + 1 == avifileinfo->nb_frames )
	return;

    if (avifileinfo)
      {
        if( !GUI_getFrame(curframe + 1,rdr_decomp_buffer,&flags))
        	{
                  GUI_Error_HIG(_("Decompressing error"),_( "Cannot decode next frame."));
           	}
           else
           {
		curframe++;
               	if(mode_preview)
          			editorUpdatePreview( curframe) ;
						
		renderUpdateImage(rdr_decomp_buffer->data);
     		 update_status_bar(rdr_decomp_buffer);

		UI_purge();
	  	}
     }
}


//_____________________________________________________________
void GUI_NextKeyFrame(void)
{

    uint32_t f;
    uint32_t flags;


    if (playing)
		return;

    f=curframe;
    if (avifileinfo)
      {
		if(!video_body->getNKFrame(&f))
		//if( !GUI_getFrameNKF(&f ,rdr_decomp_buffer))
        	{
            //	GUI_Error_HIG("Decompressing error", NULL);
           	}
		else
		{
			curframe=f;
			if( !GUI_getFrame(curframe,rdr_decomp_buffer,&flags))
        		{
                          GUI_Error_HIG(_("Decompressing error"),_( "Cannot decode keyframe."));
			}
			
			renderUpdateImage(rdr_decomp_buffer->data);
			if(mode_preview)
         				editorUpdatePreview( curframe)     ;

			update_status_bar(rdr_decomp_buffer);
			UI_purge();
		}
		
  	}

}

//**********************************************************************

uint8_t  countLightPixels(int darkness)
{

    uint32_t width = avifileinfo->width;
    uint32_t height = avifileinfo->height;
    uint32_t sz = width * height ;
    const int maxnonb=(width* height)>>8;

    uint8_t *buff;

    int cnt4=0;

    buff=rdr_decomp_buffer->data+ sz;

    while(--buff>rdr_decomp_buffer->data)
    {
      if(*buff > darkness )
	cnt4++;
	if(cnt4>=maxnonb)
	  return(1);
    }

    return(0);
}
//**********************************************************************
// Fast version to decide if a frame is black or not
// Return 0 if the frame is black
// return 1 else
//**********************************************************************

static const int  sliceOrder[8]={3,4,2,5,1,6,0,7};

static int sliceScanNotBlack(int darkness, int maxnonb, int sliceNum)
{

{

    uint32_t width = avifileinfo->width;
    uint32_t height = avifileinfo->height>>3;
    uint32_t sz = width * height ;    

    uint8_t *buff,*start;

    int cnt4=0;

    start=rdr_decomp_buffer->data+ sz*sliceNum;
    buff=start+sz;

    while(--buff>start)
    {
      if(*buff > darkness )
      {
        cnt4++;
        if(cnt4>=maxnonb)
          return(1);
      }
    }
    //printf("Slice : %d count:%d max:%d\n",sliceNum,cnt4,maxnonb);
    return(0);
}

}
uint8_t  fastIsNotBlack(int darkness)
{

    uint32_t width = avifileinfo->width;
    uint32_t height = avifileinfo->height;    
    uint32_t maxnonb=(width* height)>>8;
    
        maxnonb>>=3;
        // Divide the screen in 8 part  : 0 1 2 3 4 5 6 7 
        // Scan 2 & 3 first, if still good, go on
        for(uint32_t i=0;i<6;i++)
        {
                if(sliceScanNotBlack(darkness,maxnonb,sliceOrder[i])) return 1;
        }
        // The slice 0 & 7 are particular and we admit twice as much
        if(sliceScanNotBlack(darkness,maxnonb*2,0)) return 1;
        if(sliceScanNotBlack(darkness,maxnonb*2,7)) return 1;

    return(0);
}


//**********************************************************************

void GUI_NextPrevBlackFrame(int dir)
{
   uint32_t f;
   uint32_t flags;
   uint16_t reresh_count=0;

    if (playing)
		return;
    if (! avifileinfo)
       return;

   const int darkness=40;

   DIA_working *work=new DIA_working("Seeking");
   while(1)
   {

   	f=curframe+dir;
   	if(work->update(1)) break;

	if((f==0 && dir==-1)|| (f==avifileinfo->nb_frames-1&&dir==1)) break;

     if( !video_body->getUncompressedFrame(f ,rdr_decomp_buffer,&flags))
       {
       		curframe=0;
		video_body->getUncompressedFrame(0 ,rdr_decomp_buffer);
		break;
       }

        curframe=f;

        if(!fastIsNotBlack(darkness)) break;
        reresh_count++;
        if(reresh_count>100)
        {
                update_status_bar(rdr_decomp_buffer);
                reresh_count=0;
        }       
   }
   delete work;
   renderUpdateImage(rdr_decomp_buffer->data);
   if(mode_preview)
	 editorUpdatePreview( curframe)     ;
   update_status_bar(rdr_decomp_buffer);

   return ;
}
uint8_t A_ListAllBlackFrames(char *name)
{
// Print a list of all black frames
//_____________________________________________________________
    uint32_t f;
    uint32_t flags;
    uint32_t startframe;
    uint16_t mm,hh,ss,ms;

    uint16_t reresh_count=0;

    char *outfile;
    FILE *fd;

    outfile=name;    

    if ( playing )
        return 0;
    if ( !avifileinfo )
        return 0;

   
    if ( !outfile )
        return 0;
    fd=fopen(outfile, "wb");
    if ( fd == NULL )
    {
        fprintf(stderr, "cannot create output file for list of black frames\n");
        return 0;
    }

    const int darkness=40;

    startframe=curframe;
    DIA_working *work=new DIA_working("Finding black frames");
    printf("\n** Listing all black frames **\n");

    for (f=0; f<avifileinfo->nb_frames; f++) {
       if( work->update( 100 * f / avifileinfo->nb_frames ) ) 
            break;
        if ( !video_body->getUncompressedFrame(f,rdr_decomp_buffer,&flags) ) {
            curframe=0;
            video_body->getUncompressedFrame(0,rdr_decomp_buffer);
            break;
        }

        curframe=f;
        if ( !fastIsNotBlack(darkness) ) {
            frame2time(curframe,avifileinfo->fps1000,&hh,&mm,&ss,&ms);
            printf("\tBlack frame: frame %d  time %02d:%02d:%02d.%03d\n", curframe, hh, mm, ss, ms);
            fprintf(fd, "\tBlack frame: frame %d  time %02d:%02d:%02d.%03d\n", curframe, hh, mm, ss, ms);
        }
        reresh_count++;
        if(reresh_count>100)
        {
                update_status_bar(rdr_decomp_buffer);
                reresh_count=0;
        }
    }

    printf("** done **\n\n");
    fclose(fd);
    delete work;
    curframe=startframe;
    if ( !video_body->getUncompressedFrame(curframe,rdr_decomp_buffer,&flags) ) {
        curframe=0;
        video_body->getUncompressedFrame(0,rdr_decomp_buffer);
    }
    renderUpdateImage(rdr_decomp_buffer->data);
    if ( mode_preview )
        editorUpdatePreview( curframe );
    update_status_bar(rdr_decomp_buffer);

    return 1;
}
//**********************************************************************

//
// Go to the neareste frame near frame
// we take the previous one to simplify
//_____________________________________________________________
void GUI_GoToKFrame(uint32_t frame)
{
   uint32_t old=curframe;
//  uint32_t flags;

        if (playing)
				return;

	if (avifileinfo)
	{
		// curframe=frame;
		//
		curframe=frame;
		GUI_PreviousKeyFrame();	
     	}
}

//_____________________________________________________________
int GUI_GoToFrame(uint32_t frame)
{
uint32_t flags;

	if (playing)
		return 0;

      if (!avifileinfo)
      		return 0;
      

	if( !GUI_getFrame(frame ,rdr_decomp_buffer,&flags))
	{
          GUI_Error_HIG(_("Decompressing error"),_( "Cannot decode the frame."));
		return 0;
	}

	curframe = frame;
	renderUpdateImage(rdr_decomp_buffer->data);

	if(mode_preview)
		editorUpdatePreview( curframe);

	update_status_bar(rdr_decomp_buffer);
	UI_purge();
	
    	return 1;

}



void GUI_PreviousKeyFrame(void)
{

 uint32_t f;
 uint32_t flags;


    if (playing)
		return;

    f=curframe;
    if (avifileinfo)
      {
		if(!video_body->getPKFrame(&f))
		//if( !GUI_getFrameNKF(&f ,rdr_decomp_buffer))
        	{
            //	GUI_Error_HIG("Decompressing error", NULL);
           	}
		else
		{
			curframe=f;
			if( !GUI_getFrame(curframe,rdr_decomp_buffer,&flags))
        		{
                          GUI_Error_HIG(_("Decompressing error"),_( "Cannot decode keyframe."));
			}
			
			renderUpdateImage(rdr_decomp_buffer->data);
			if(mode_preview)
         				editorUpdatePreview( curframe)     ;

			update_status_bar(rdr_decomp_buffer);
			UI_purge();
		}
		
  	}
};

uint8_t A_rebuildKeyFrame(void)
{

        return video_body->rebuildFrameType();
}