/***************************************************************************
                          ADM_pics.cpp  -  description
                             -------------------

                             Open a bunch of bmps and read them as a movie
                             Useful for people doing raytracing or doing img/img
                             modifications


    begin                : Tue Jun 4 2002
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
#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_inpics/ADM_pics.h"
#include "ADM_inpics/bitmap.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_INPIC
#include "ADM_toolkit/ADM_debug.h"

static uint16_t s16;
static uint32_t s32;
#define MAX_ACCEPTED_OPEN_FILE 99999

//****************************************************************
uint8_t
    picHeader::getFrameNoAlloc(uint32_t framenum, uint8_t * ptr,
			       uint32_t * framelen, uint32_t * flags)
{
    *flags = AVI_KEY_FRAME;
    return getFrameNoAlloc(framenum, ptr, framelen);
}
//****************************************************************
uint8_t
    picHeader::getFrameNoAlloc(uint32_t framenum, uint8_t * ptr,
			       uint32_t * framelen)
{
    ADM_assert(framenum <= _nb_file);

    fseek(_fd[framenum], _offset, SEEK_SET);
    fread(ptr, _imgSize[framenum] - _offset, 1, _fd[framenum]);
    *framelen = _imgSize[framenum] - _offset;;
    return 1;
}
//****************************************************************
uint8_t picHeader::close(void)
{
    if (_fd) {
	for (uint32_t i = 0; i < _nb_file; i++) {
	    fclose(_fd[i]);
	}
	ADM_dealloc(_fd);
	_fd = NULL;

    }
    if (_imgSize) {
	delete[]_imgSize;
	_imgSize = NULL;
    }
    return 0;
}
//****************************************************************
/*
	Open a bunch of images


*/


uint32_t picHeader::read32(FILE * fd)
{
    uint32_t i;
    i = 0;
    i = (read8(fd) << 24) + (read8(fd) << 16) + (read8(fd) << 8) +
	(read8(fd));
    return i;

}

uint16_t picHeader::read16(FILE * fd)
{
    uint16_t i;

    i = 0;
    i = (read8(fd) << 8) + (read8(fd));
    return i;
}

uint8_t picHeader::read8(FILE * fd)
{
    uint8_t i;
    ADM_assert(fd);
    i = 0;
    if (!fread(&i, 1, 1, fd)) {
	printf("\n Problem reading the file !\n");
    }
    return i;
}
//****************************************************************
uint8_t picHeader::open(char *inname)
{

    uint32_t nnum;
    uint32_t *fcc;
    uint8_t fcc_tab[4];
    FILE *fd;
    char *end;
    uint32_t w = 0, h = 0;

    // 1- identity the file type
    //
    fcc = (uint32_t *) fcc_tab;
    fd = fopen(inname, "rb");
    if (!fd) {
	printf("\n Cannot open that file!\n");
	return 0;
    }
    fread(fcc_tab, 4, 1, fd);
    fclose(fd);
    if (fourCC::check(*fcc, (uint8_t *) "RIFF")) {
	_type = PIC_BMP;
	printf("\n It looks like BMP (RIFF)...\n");
    } else {
	if (fcc_tab[0] == 'B' && fcc_tab[1] == 'M') {
	    _type = PIC_BMP2;
	    printf("\n It looks like BMP (BM)...\n");
	} else if (fcc_tab[0] == 0xff && fcc_tab[1] == 0xd8) {
	    _type = PIC_JPEG;
	    printf("\n It looks like Jpg...\n");
	} else {
	    if (fcc_tab[1] == 'P' && fcc_tab[2] == 'N'
		&& fcc_tab[3] == 'G') 
		{
    		printf("\n It looks like PNG...\n");
		    _type = PIC_PNG;
	    } else {
		printf("\n Cannot identify file (%x %x)\n", *fcc,
		       *fcc & 0xffff);
		return 0;
	    }
	}
    }

    // Then spit the name in name and extension
    char *name;
    char *extension;
    PathSplit(inname, &name, &extension);


    nnum = 1;

    end = name + strlen(name) - 1;
    while ((*end >= '0') && (*end <= '9')) {
	end--;
	nnum++;
    };
char realname[250];
char realstring[250];

    if (nnum == 1) {
	printf("\n only one file!");
        _nb_file=1;
         sprintf(realstring, "%%s.%s",extension);
    }
    else
    {
    nnum--;
    end++;
    _first = atoi(end);
    printf("\n First : %lu, num digit :%lu", _first, nnum);
    *(end) = 0;
    printf("\n Path : %s\n", name);

    
    sprintf(realstring, "%%s%%0%lud.%s", nnum, extension);
    printf("\n string : %s", realstring);

    _nb_file = 0;

    for (uint32_t i = 0; i < MAX_ACCEPTED_OPEN_FILE; i++) {
	sprintf(realname, realstring, name, i + _first);
	printf("\n %lu : %s", i, realname);
	fd = fopen(realname, "rb");
	if (fd == NULL)
	    break;
	fclose(fd);
	_nb_file++;
    }
    }
    printf("\n found %lu images\n", _nb_file);


    _fd = (FILE **) ADM_alloc(_nb_file * sizeof(FILE *));
    _imgSize = new uint32_t[_nb_file];
    //_________________________________
    // now open them and assign fd && imgSize
    //__________________________________
    for (uint32_t i = 0; i < _nb_file; i++) {
	sprintf(realname, realstring, name, i + _first);
	printf("\n %lu : %s", i, realname);
	_fd[i] = fopen(realname, "rb");
	fseek(_fd[i], 0, SEEK_END);
	_imgSize[i] = ftell(_fd[i]);
	fseek(_fd[i], 0, SEEK_SET);
	ADM_assert(_fd[i] != NULL);
    }

    delete[]name;
    delete[]extension;


    //
    //      Image is bmp type
    //________________________
    switch (_type) {
    case PIC_BMP:
	{
	    BITMAPHEADER bmph;

	    fread(&s16, 2, 1, _fd[0]);
	    if (s16 != 0x4D42) {
		printf("\n incorrect bmp sig.\n");
		return 0;
	    }
	    fread(&s32, 4, 1, _fd[0]);
	    fread(&s32, 4, 1, _fd[0]);
	    fread(&s32, 4, 1, _fd[0]);
	    fread(&bmph, sizeof(bmph), 1, _fd[0]);
	    if (bmph.compressionScheme != 0) {
		printf("\ncannot handle compressed bmp\n");
		return 0;
	    }
	    _offset = bmph.size + 14;
	    w = bmph.width;
	    h = bmph.height;
	}
	break;


	//Retrieve width & height
	//_______________________
    case PIC_JPEG:
	{
	    uint16_t tag = 0, count = 0, off;

	    _offset = 0;
	    fseek(_fd[0], 0, SEEK_SET);
	    read16(_fd[0]);	// skip jpeg ffd8
	    while (count < 10 && tag != 0xFFC0) {

		tag = read16(_fd[0]);
		if ((tag >> 8) != 0xff) {
		    printf("invalid jpeg tag found (%x)\n", tag);
		}
		if (tag == 0xFFC0) {
		    read16(_fd[0]);	// size
		    read8(_fd[0]);	// precision
		    h = read16(_fd[0]);
		    w = read16(_fd[0]);
		} else {

		    off = read16(_fd[0]);
		    if (off < 2) {
			printf("Offset too short!\n");
			return 0;
		    }
		    aprintf("Found tag : %x , jumping %d bytes\n", tag,
			    off);
		    fseek(_fd[0], off - 2, SEEK_CUR);
		}
		count++;
	    }
	    if (tag != 0xffc0) {
		printf("Cannot fint start of frame\n");
		return 0;
	    }
	    printf("\n %lu x %lu..\n", w, h);
	}
	break;

    case PIC_BMP2:
	{
	    BITMAPINFOHEADER bmph;

	    fseek(_fd[0], 10, SEEK_SET);

#define MK32() (fcc_tab[0]+(fcc_tab[1]<<8)+(fcc_tab[2]<<16)+ \
						(fcc_tab[3]<<24))

	    fread(fcc_tab, 4, 1, _fd[0]);
	    _offset = MK32();
	    // size, width height follow as int32 
	    fread(&bmph, sizeof(bmph), 1, _fd[0]);
#ifdef ADM_BIG_ENDIAN
	    Endian_BitMapInfo(&bmph);
#endif
	    if (bmph.biCompression != 0) {
		printf("\ncannot handle compressed bmp\n");
		return 0;
	    }
	    w = bmph.biWidth;
	    h = bmph.biHeight;
	    printf("W: %d H: %d offset : %d\n", w, h, _offset);
	}

	break;
	
	case PIC_PNG:
	    {
    	     _offset = 0;
	         fseek(_fd[0], 0, SEEK_SET);
	         read32(_fd[0]);
	         read32(_fd[0]);
	         read32(_fd[0]);
	         read32(_fd[0]);
    	     w=read32(_fd[0]);;
    	     h=read32(_fd[0]);;
    	     // It is big endian
    	     printf("Png seems to be %d x %d \n",w,h);
    	     break;
	    }
	    break;
    default:
	ADM_assert(0);
    }

//_______________________________________
//              Now build header info
//_______________________________________
    _isaudiopresent = 0;	// Remove audio ATM
    _isvideopresent = 1;	// Remove audio ATM

#define CLR(x)              memset(& x,0,sizeof(  x));

    CLR(_videostream);
    CLR(_mainaviheader);

    _videostream.dwScale = 1;
    _videostream.dwRate = 25;
    _mainaviheader.dwMicroSecPerFrame = 40000;;	// 25 fps hard coded
    _videostream.fccType = fourCC::get((uint8_t *) "vids");

    _video_bih.biBitCount = 24;

    _videostream.dwLength = _mainaviheader.dwTotalFrames = _nb_file;
    _videostream.dwInitialFrames = 0;
    _videostream.dwStart = 0;
    //
    //_video_bih.biCompression= 24;
    //
    _video_bih.biWidth = _mainaviheader.dwWidth = w;
    _video_bih.biHeight = _mainaviheader.dwHeight = h;
    //_video_bih.biPlanes= 24;
    switch(_type)
    {
        case PIC_JPEG:
	        _video_bih.biCompression = _videostream.fccHandler =
	                fourCC::get((uint8_t *) "MJPG");
	        break;
	    case PIC_BMP:
	    case PIC_BMP2:
	        _video_bih.biCompression = _videostream.fccHandler = 0;
	        break;
	    case PIC_PNG:
	        _video_bih.biCompression = _videostream.fccHandler =
	                fourCC::get((uint8_t *) "PNG ");
	        break;
        default:
            ADM_assert(0);
    }
    printf("Offset : %lu\n", _offset);
    return 1;
}
//****************************************************************
uint8_t picHeader::setFlag(uint32_t frame, uint32_t flags)
{
    UNUSED_ARG(frame);
    UNUSED_ARG(flags);
    return 0;
}
//****************************************************************
uint32_t picHeader::getFlags(uint32_t frame, uint32_t * flags)
{
    UNUSED_ARG(frame);
    *flags = AVI_KEY_FRAME;
    return 1;
}
//****************************************************************
//EOF
