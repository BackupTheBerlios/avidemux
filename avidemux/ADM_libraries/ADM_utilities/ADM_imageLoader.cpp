/***************************************************************************
    copyright            : (C) 2007 by mean
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ADM_assert.h"
#include "default.h"

#include "ADM_lavcodec.h"
#include "ADM_utilities/default.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_image.h"
#include "ADM_osSupport/ADM_cpuCap.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_ffmp43.h"
#include "ADM_toolkit/toolkit.hxx"
//**********************************
static ADMImage *createImageFromFile_jpeg(const char *filename);
//***********************************
static uint8_t read8(FILE *fd)
{
	return fgetc(fd);
}
static uint32_t read16(FILE *fd)
{
	uint32_t a,b;
	
	a=fgetc(fd);
	b=fgetc(fd);
	return (a<<8)+b;
		
}
/*
		\fn 	createImageFromFile
		\brief 	Create and returns an ADMImage from a file, only YV12 jpg supported ATM

*/
ADMImage *createImageFromFile(const char *filename)
{
	    uint32_t *fcc;
	    uint8_t fcc_tab[4];
	    FILE *fd;

	    // 1- identity the file type
	    //
	    fcc = (uint32_t *) fcc_tab;
	    fd = fopen(filename, "rb");
	    if (!fd) 
	    {
	    	printf("[imageLoader] Cannot open that file!\n");
	    	return NULL;
	    }
	    fread(fcc_tab, 4, 1, fd);
	    fclose(fd);
	    if (fcc_tab[0] == 0xff && fcc_tab[1] == 0xd8) 
	    {
		   	return createImageFromFile_jpeg(filename);
		} 
	    printf("[imageLoader] Unrecognized file!\n");
	    return NULL;
}
/**
 * 	\fn createImageFromFile_jpeg
 *  \brief Create image from jpeg file
 */
ADMImage *createImageFromFile_jpeg(const char *filename)
{
	
	FILE *fd;
	uint32_t _imgSize;
	uint32_t w = 0, h = 0;
	   

		fd = fopen(filename, "rb");
		fseek(fd, 0, SEEK_END);
		_imgSize = ftell(fd);
		fseek(fd, 0, SEEK_SET);

		//Retrieve width & height
		//_______________________
		    uint16_t tag = 0, count = 0, off;

		    
		    fseek(fd, 0, SEEK_SET);
		    read16(fd);	// skip jpeg ffd8
		    while (count < 10 && tag != 0xFFC0) 
		    {

		    	tag = read16(fd);
		    	if ((tag >> 8) != 0xff) 
		    	{
		    		printf("[imageLoader]invalid jpeg tag found (%x)\n", tag);
		    	}
		    	if (tag == 0xFFC0) 
		    	{
		    		read16(fd);	// size
		    		read8(fd);	// precision
		    		h = read16(fd);
		    		w = read16(fd);
	                if(w&1) w++;
	                if(h&1) h++;
		    	} 
		    	else 
		    	{
		    		off = read16(fd);
		    		if (off < 2) 
		    		{
		    			printf("[imageLoader]Offset too short!\n");
		    		    fclose(fd);
		    		    return NULL;
		    		}
		    		fseek(fd, off - 2, SEEK_CUR);
		    	}
			count++;
		    }
		    if (tag != 0xffc0) 
		    {
		    	printf("[imageLoader]Cannot fint start of frame\n");
				fclose(fd);
				return NULL;
		    }
		    printf("[imageLoader] %lu x %lu.., total Size : %u, offset %u\n", w, h,_imgSize,off);
		    
		// Load the binary coded image
		    uint8_t *data=new uint8_t[_imgSize];
		    fseek(fd, 0, SEEK_SET);
		    fread(data,_imgSize,1,fd);
		    fclose(fd);
		  //
		    
		    ADMImage tmpImage(w,h,1); // It is a reference image
		    // Now unpack it ...
		    decoderFFMJPEG *decoder=new decoderFFMJPEG(w,h);
		    ADMCompressedImage bin;
		    bin.data=data;
		    bin.dataLength=_imgSize; // This is more than actually, but who cares...
		    
		    decoder->uncompress (&bin, &tmpImage);
		    //
		    ADMImage *image=NULL;
		    switch(tmpImage._colorspace)
		    {
		    case ADM_COLOR_YV12:
		    {
		    	printf("[imageLoader] YV12\n");
	    		image=new ADMImage(w,h);
	    		image->duplicate(&tmpImage);
	    		break;
		    }
		    case ADM_COLOR_YUV422:
		    {
		    	printf("[imageLoader] YUY2\n");
		    	image=new ADMImage(w,h);
		    	COL_422_YV12(tmpImage._planes, tmpImage._planeStride,  image->data,w,h);
		    	break;
		    }
		    default:
		    	GUI_Error_HIG(_("Wrong Colorspace"),_("Only YV12/I420 or YUY2/I422 JPegs are supported"));
		    }
		    // Cannot destroy decoder earlier as tmpImage has pointers to its internals
		    delete decoder;
		    decoder=NULL;
		    delete [] data;
		    return image;		
}

//EOF
